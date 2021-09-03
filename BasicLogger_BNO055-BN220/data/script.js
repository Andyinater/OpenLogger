/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-mpu-6050-web-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

let scene, camera, rendered, cube, resetQuat, rqx, rqy, rqz, rqw, beenReset;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}
function createCar() {
  const car = new THREE.Group();

  const backWheel = createWheels();
  backWheel.position.y = 6;
  backWheel.position.x = -18;
  car.add(backWheel);

  const frontWheel = createWheels();
  frontWheel.position.y = 6;
  frontWheel.position.x = 18;
  car.add(frontWheel);

  const main = new THREE.Mesh(
    new THREE.BoxBufferGeometry(60, 15, 30),
    new THREE.MeshLambertMaterial({ color: 0xa52523 })
  );
  main.position.y = 12;
  car.add(main);

  const carFrontTexture = getCarFrontTexture();

  const carBackTexture = getCarFrontTexture();

  const carRightSideTexture = getCarSideTexture();

  const carLeftSideTexture = getCarSideTexture();
  carLeftSideTexture.center = new THREE.Vector2(0.5, 0.5);
  carLeftSideTexture.rotation = Math.PI;
  carLeftSideTexture.flipY = false;

  const cabin = new THREE.Mesh(new THREE.BoxBufferGeometry(33, 12, 24), [
    new THREE.MeshLambertMaterial({ map: carFrontTexture }),
    new THREE.MeshLambertMaterial({ map: carBackTexture }),
    new THREE.MeshLambertMaterial({ color: 0xffffff }), // top
    new THREE.MeshLambertMaterial({ color: 0xffffff }), // bottom
    new THREE.MeshLambertMaterial({ map: carRightSideTexture }),
    new THREE.MeshLambertMaterial({ map: carLeftSideTexture })
  ]);
  cabin.position.x = -6;
  cabin.position.y = 25.5;
  car.add(cabin);

  return car;
}

function createWheels() {
  const geometry = new THREE.BoxBufferGeometry(12, 12, 33);
  const material = new THREE.MeshLambertMaterial({ color: 0x333333 });
  const wheel = new THREE.Mesh(geometry, material);
  return wheel;
}

function getCarFrontTexture() {
  const canvas = document.createElement("canvas");
  canvas.width = 64;
  canvas.height = 32;
  const context = canvas.getContext("2d");

  context.fillStyle = "#ffffff";
  context.fillRect(0, 0, 64, 32);

  context.fillStyle = "#666666";
  context.fillRect(8, 8, 48, 24);

  return new THREE.CanvasTexture(canvas);
}

function getCarSideTexture() {
  const canvas = document.createElement("canvas");
  canvas.width = 128;
  canvas.height = 32;
  const context = canvas.getContext("2d");

  context.fillStyle = "#ffffff";
  context.fillRect(0, 0, 128, 32);

  context.fillStyle = "#666666";
  context.fillRect(10, 8, 38, 24);
  context.fillRect(58, 8, 60, 24);

  return new THREE.CanvasTexture(canvas);
}


function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  /*camera = new THREE.PerspectiveCamera(85, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);*/
  
  camera = new THREE.OrthographicCamera(parentWidth(document.getElementById("3Dcube"))/-200, parentWidth(document.getElementById("3Dcube"))/200, parentHeight(document.getElementById("3Dcube"))/200, parentHeight(document.getElementById("3Dcube"))/-200, 0.1, 1000);
  
  //camera = new THREE.OrthographicCamera(-20, 20, 30, -30, 0.1, 1000);
  camera.position.z = 5;
	// Set up lights
	
	const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
	scene.add(ambientLight);
	/*
	const dirLight = new THREE.DirectionalLight(0xffffff, 0.8);
	dirLight.position.set(200, 500, 300);
	scene.add(dirLight);
	*/

	// Set up camera
	//const aspectRatio = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"))
	//const cameraWidth = 300;
	//const cameraHeight = cameraWidth / aspectRatio;

	//camera = new THREE.OrthographicCamera(cameraWidth / -2, cameraWidth / 2, cameraHeight / 2, cameraHeight / -2, 0,1000 );
	//camera.position.set(-200, 150, 0);
	//camera.position.set(0, 0, 5);
	//camera.lookAt(0, 10, 0);
	


  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);
  document.getElementById("gyroX").innerHTML = 666;


  // Create main box
  const firstgeometry = new THREE.BoxGeometry(1, 0.5, 3);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0xff0000}),
    new THREE.MeshBasicMaterial({color:0xffea00}),
    new THREE.MeshBasicMaterial({color:0x2fff00}),
    new THREE.MeshBasicMaterial({color:0x00fff7}),
    new THREE.MeshBasicMaterial({color:0x001eff}),
    new THREE.MeshBasicMaterial({color:0xff00ff}),
  ];

  const firstmaterial = new THREE.MeshFaceMaterial(cubeMaterials);
/*  prebody = new THREE.Mesh(firstgeometry, firstmaterial);
  
  // Create cabin box
  const secondgeometry = new THREE.BoxGeometry(1/1.5, 0.5*2, 3/2);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
  ];

  const secondmaterial = new THREE.MeshFaceMaterial(cubeMaterials);
  precabin = new THREE.Mesh(secondgeometry, secondmaterial);
  
  //merge 2
  var singleGeometry = new THREE.Geometry();
  singleGeometry.merge(prebody.geometry, prebody.matrix);
  singleGeometry.merge(precabin.geometry, precabin.matrix);
  var newmat = new THREE.MeshPhongMaterial({color: 0xFF0000});
  
  cube = new THREE.Mesh(singleGeometry, newmat);
*/

  //cube = createCar();

  cube = new THREE.Mesh(firstgeometry, firstmaterial);
  scene.add(cube);
  
  beenReset = false;
  resetQuat = new THREE.Quaternion(0,0,0,1);
  rqx = 0;
  rqy = 0;
  rqz = 0;
  rqw = 0;
  
  /*cube = createCar();
  scene.add(cube);
  */
  
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

// Create the 3D representation
init3D();

// Create events for the sensor readings
if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('gyro_readings_euler', function(e) {
    //console.log("gyro_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("gyroX").innerHTML = obj.gyroX;
    document.getElementById("gyroY").innerHTML = obj.gyroY;
    document.getElementById("gyroZ").innerHTML = obj.gyroZ;

    // Change cube rotation after receiving the readinds
    // cube.rotation.x = obj.gyroY;
    // cube.rotation.z = obj.gyroX;
    // cube.rotation.y = obj.gyroZ;
    // renderer.render(scene, camera);
  }, false);
  
  source.addEventListener('gyro_readings_quat', function(e) {
    //console.log("gyro_readings", e.data);
    var obj = JSON.parse(e.data);
	document.getElementById("quatW").innerHTML = obj.quatW;
    document.getElementById("quatX").innerHTML = obj.quatX;
    document.getElementById("quatY").innerHTML = obj.quatY;
    document.getElementById("quatZ").innerHTML = obj.quatZ;

	/*
	// Change cube quaternion after receiving data, strange necessary rotation
	var q = new THREE.Quaternion(-1*obj.quatX, obj.quatZ, obj.quatY, obj.quatW);
	rqx = -1*obj.quatX;
	rqy = obj.quatZ;
	rqz = obj.quatY;
	rqw = obj.quatW;
	*/
	
	// Change cube quaternion after receiving data, strange necessary rotation
	var q = new THREE.Quaternion(-1*obj.quatX, obj.quatZ, obj.quatY, obj.quatW);
	rqx = -1*obj.quatX;
	rqy = obj.quatZ;
	rqz = obj.quatY;
	rqw = obj.quatW;
	
	//var q2 = new THREE.Quaternion(1,0,0,0);
	//cube.quaternion.multiplyQuaternions(q,q2);
	if (beenReset) {
		cube.quaternion.multiplyQuaternions(resetQuat,q);
	} else {
		cube.quaternion.copy(q)
	}
	//cube.quaternion.multiplyQuaternions(resetQuat,q);
	cube.quaternion.normalize();
	//q.normalize();
	//cube.rotation.setFromQuaternion(q);
	//cube.quaternion.set(obj.quatX, obj.quatY, obj.quatZ, obj.quatW);
	//cube.quaternion.normalize();
	//cube.quaternion.rotateTowards(q,0.1);
	renderer.render(scene, camera);
	
    // Change cube rotation after receiving the readinds
    //cube.rotation.x = obj.gyroY;
    //cube.rotation.z = obj.gyroX;
    //cube.rotation.y = obj.gyroZ;
    //renderer.render(scene, camera);
  }, false);

  source.addEventListener('temperature_reading', function(e) {
    console.log("temperature_reading", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);

  source.addEventListener('accelerometer_readings', function(e) {
    console.log("accelerometer_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("accX").innerHTML = obj.accX;
    document.getElementById("accY").innerHTML = obj.accY;
    document.getElementById("accZ").innerHTML = obj.accZ;
  }, false);
}

function resetPosition(){
  //var xhr = new XMLHttpRequest();
  //xhr.open("GET", "/"+element.id, true);
  //console.log(element.id);
  //xhr.send();
  
  beenReset = true;
  
  var calibQuat = new THREE.Quaternion();
  
  
  //calibQuat.set(cube.quaternion.x,cube.quaternion.y,cube.quaternion.z,cube.quaternion.w);
  calibQuat.set(-1*rqx,-1*rqy,-1*rqz,rqw);
  //calibQuat.set(rqx,rqy,rqz,rqw);
  calibQuat.normalize();
  document.getElementById("gyroX").innerHTML = 67;
  var checkAns = document.querySelector('input[name = "startingOrientation"]:checked').value;
  document.getElementById("gyroX").innerHTML = 67.25;
  document.getElementById("gyroX").innerHTML = true;
  document.getElementById("gyroY").innerHTML = checkAns;
  document.getElementById("gyroX").innerHTML = 67.5;
  
  
  if (checkAns == 1){
	  document.getElementById("gyroX").innerHTML = 66;
	  var standardQuat = new THREE.Quaternion(0,0,0,1);
	  
  } 
  else {
	  document.getElementById("gyroX").innerHTML = 65;
	  var standardQuat = new THREE.Quaternion(0,1,0,0);
	  
  } 
  
  //var standardQuat = new THREE.Quaternion(0,0,0,1);
  document.getElementById("gyroX").innerHTML = 68;
  document.getElementById("gyroY").innerHTML = resetQuat.y;
  document.getElementById("gyroZ").innerHTML = calibQuat.z;
  //resetQuat = new THREE.Quaternion(0,0,0,1);
  resetQuat = new THREE.Quaternion();
  
  //calibQuat.invert();
  resetQuat.multiplyQuaternions(standardQuat, calibQuat);
  
  
  //resetQuat.multiplyQuaternions(calibQuat,standardQuat);
  resetQuat.normalize();
  //resetQuat.multiplyQuaternions(standardQuat, calibQuat.inverse());
  document.getElementById("gyroX").innerHTML = 69;
  document.getElementById("gyroX").innerHTML = resetQuat.x;
  document.getElementById("gyroY").innerHTML = resetQuat.y;
  document.getElementById("gyroZ").innerHTML = resetQuat.z;
  
}
