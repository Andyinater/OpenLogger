% Define Simulation Parameters
dt = 0.02;
NoiseGain_r = 1;
NoiseGain_ay = 20;
CovarShape = [1 0; 0 1];

% Define Vehicle Parameters
cf = 80000;
cr = 80000;
I = 2114;
E_I = 0; % optional parameter error
b = 1.041;
a = 1.628;
m = 1251;
E_m = 0; % optional parameter error
u = 10;
E_u = 0; % optional parameter error



% Create time series with associated inputs
t = 0:dt:4;
% U = 5*(2*pi()/360)*ones(size(t));
U = sin(2*pi*0.5*t);

% Create system matrices
% Create initial state and covariance matrices
X_0 = [0;0];
P_0 = 1*CovarShape;

%Process error matrix
Q = 0.01*CovarShape;

%Observation error matrix
R = 0.01*CovarShape;

% Ground truth bicycle model
A_GT = [-(cf+cr)/((m+E_m)*(u+E_u)) -(u+E_u)-(a*cf-b*cr)/((m+E_m)*(u+E_u));
        -(a*cf-b*cr)/((I+E_I)*(u+E_u)) -((a^2)*cf+(b^2)*cr)/((I+E_I)*(u+E_u)) ];
    
B_GT = [cf/(m+E_m); a*cf/(I+E_I)];

% Outputs IMU measures r and ay
% ay = v_dot + ru
C_GT = [0 1;
        A_GT(1,1) A_GT(1,2)+(u+E_u)]; %[r;ay]

% v_dot contribution from U
D_GT = [0; 
        B_GT(1)];

% Kalman bicycle model
A = [-(cf+cr)/((m)*(u)) -(u)-(a*cf-b*cr)/((m)*(u));
        -(a*cf-b*cr)/((I)*(u)) -((a^2)*cf+(b^2)*cr)/((I)*(u)) ];

B = [cf/(m); a*cf/(I)];

% Outputs are states 
C = [1 0;
     0 1];

% U does not change state
D = [0;
    0];


% Create continuous and discretized systems
sys = ss(A_GT,B_GT,C_GT,D_GT); % outputs IMU measurements for pseudo-measurements [r;ay]
sys_states = ss(A_GT,B_GT,[1 0;0 1],[0;0]); % Outputs system state [v;r]
sys_dots = ss(A_GT,B_GT,A_GT,B_GT)
ksys = ss(A,B,C,D); % Outputs states [v;r]
sysD = c2d(ksys,dt); 
Adt = sysD.A;
Bdt = sysD.B;
Cdt = sysD.C;
Ddt = sysD.D;



% Create ground truth and measurements series
y1 = lsim(sys_states,U,t); % state series from GT [v;r]
y2 = lsim(sys,U,t); % pseudo-IMU measurement series from GT [r;ay]


% Add gaussian white noise to the measurements series
y2n = [y2(:,1)+sqrt(NoiseGain_r)*randn(size(y2,1),1) y2(:,2)+sqrt(NoiseGain_ay)*randn(size(y2,1),1)]; 




% Create Kalman Loop
% Set initial values
X_k0 = X_0;
P_k0 = P_0;

% Hold Predictions Matrix
K_preds = zeros(size(y2n));
K_Xs = zeros(size(y2n));
K_dots = zeros(size(y2n));
GT_dots = zeros(size(y2n));
K11_gains = zeros(size(y2n));
K12_gains = zeros(size(y2n));
K21_gains = zeros(size(y2n));
K22_gains = zeros(size(y2n));
P_kps = zeros(size(y2n,1),4);
P_k0s = zeros(size(y2n,1),4);
for i = 1:size(U,2) % For every input
    
    
    % State Extrapolation
    % X_kp1 = A*X_k0 + B*U_k1 + w_k
    X_kp1 = Adt*X_k0 + Bdt*U(i);
    
    % P_kp = A*P_k0*A' + Qk
    P_kp = Adt*P_k0*transpose(Adt) + Q;
    P_kps(i,:) = [P_kp(1,:) P_kp(2,:)];
    
    % Determine Kalman Gain
    % K = (P_kp*C')/(C*P_kp*C' + R)
    K = (P_kp*transpose(Cdt))*inv(Cdt*P_kp*transpose(Cdt) + R);
    K11_gains(i,:) = K(1,1);
    K12_gains(i,:) = K(1,2);
    K21_gains(i,:) = K(2,1);
    K22_gains(i,:) = K(2,2);
    
    % State Correction
    % X_k1 = X_Kp1 + K[Y_k - C*X_kp]
    % must convert IMU measurement of r and ay to state [v;r] to determine
    % error/innovation between system extrapolation and measurement
    % implication
    % ay = v_dot + ru
    % v_dot = ay - ru
    % v_dot = A11*v + A12*r + B11*u
    % v = (v_dot - A12*r - B11*u)/(A11)
    % v = (ay - ru - A12*r - B11*U)/A11
    v_measured = (y2n(i,2) - y2n(i,1)*u - A_GT(1,2)*y2n(i,1) - B_GT(1,1)*U(i))/A_GT(1,1);
    
    X_k1 = X_kp1 + K*([v_measured;y2n(i,1)] - Cdt*X_kp1); % [v;r]measured - [v;r]extrapolated
    K_Xs(i,:) = X_k1;
    
    %         r             A11*v        A12*r                B11*U     r*u
    Y_k1 = [X_k1(2); A_GT(1,1)*X_k1(1) + A_GT(1,2)*X_k1(2) + B_GT(1)*U(i) + X_k1(2)*u]; % from corrected state determine kalman predicted measurement [r;ay]
    K_preds(i,:) = transpose(Y_k1); % [r;ay]
    k_pDot = Adt*X_k1 + Bdt*U(i);% Kalman prediction of state change
    K_dots(i,:) = transpose(k_pDot);
    GT_dot = Adt*transpose(y1(i,:)) + Bdt*U(i);
    GT_dots(i,:) = transpose(GT_dot);
    
    % Current Becomes Previous
    X_k0 = X_k1;
    P_k0 = (eye(2)-K*Cdt)*P_kp;
    P_k0s(i,:) = [P_k0(1,:) P_k0(2,:)]; 
end


% Plot ground truth and noisy measurements and kalman predictions
figure('Name','Kalman Predicted Measurements vs time')
plot(t,y2,'-');
hold on
plot(t,y2n,'x');
plot(t,K_preds,'--')
legend('GT_r','GT_a_y','M_r','M_a_y','K_1_,_r','K_2_,_a_y');

figure('Name','Kalman Predicted States vs time')
plot(t,y1,'-');
hold on
plot(t,K_Xs,'--');
legend('GT_v','GT_r','K_v','K_r');

figure('Name','Kalman Predicted Dots vs time')
plot(t,GT_dots,'-')
hold on
plot(t,K_dots,'--')
legend('GT_v_-_d_o_t','GT_r_-_d_o_t','K_v_-_d_o_t','K_v_-_d_o_t')


figure('Name','Kalman Gain vs time')
plot(t,K11_gains)
hold on
plot(t,K12_gains)
plot(t,K21_gains)
plot(t,K22_gains)
legend('K11','K12','K21','K22')

figure('Name','P_kp vs time')
plot(t,P_kps)

figure('Name','P_k0 vs time')
plot(t,P_k0s)