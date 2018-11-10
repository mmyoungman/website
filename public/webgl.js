'use strict';

main();

var mouseX = 0;
var mouseY = 0;
var pageX = 1;
var pageY = 1;

function mouseClick(event) {
   mouseX = event.pageX;
   mouseY = event.pageY;
   pageX = document.documentElement.clientWidth;
   pageY = document.documentElement.clientHeight;

   var coords = "X coords: " + mouseX + ", Y coords: " + mouseY;
   document.getElementById("mousePos").innerHTML = coords;
   main();
}

function createShader(gl, type, source) {
   var shader = gl.createShader(type);
   gl.shaderSource(shader, source);
   gl.compileShader(shader);
   var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
   if (success) {
      return shader;
   }

   console.log(gl.getShaderInfoLog(shader));
   gl.deleteShader(shader);
}

function createProgram(gl, vertexShader, fragmentShader) {
   var program = gl.createProgram();
   gl.attachShader(program, vertexShader);
   gl.attachShader(program, fragmentShader);
   gl.linkProgram(program);
   var success = gl.getProgramParameter(program, gl.LINK_STATUS);
   if (success) {
      return program;
   }

   console.log(gl.getProgramInfoLog(program));
   gl.deleteProgram(program);
}

function main() {
   const canvas = document.getElementById("c");
   canvas.width = document.documentElement.clientWidth;
   canvas.height = document.documentElement.clientHeight;
   const gl = canvas.getContext("webgl");

   if (gl === null) {
      alert("Unable to initialize WebGL!");
      return;
   }

   var vPromise = fetch('./shaders/2d-vertex-shader.glsl')
      .then(response => {
         return response.text()
      });

   var fPromise = fetch('./shaders/2d-fragment-shader.glsl')
      .then(response => {
         return response.text()
      });

   var vertexShaderSource = "nope";
   var fragmentShaderSource = "nope";
   Promise.all([vPromise, fPromise])
      .then(values => {
         vertexShaderSource = values[0];
         fragmentShaderSource = values[1];
      })
      .catch(error => {
         console.log(`Error: ${error}`);
      });

   function waitForShaderData(callback) {
      if(vertexShaderSource == "nope" || fragmentShaderSource == "nope") {
         window.setTimeout(waitForShaderData.bind(null, callback), 100);
      } else {
         callback();
      }
   }

   waitForShaderData(() => {
      gl.clearColor(0, 0, 0, 0);
      gl.clear(gl.COLOR_BUFFER_BIT);

      var vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
      var fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

      var program = createProgram(gl, vertexShader, fragmentShader);

      var positionAttributeLocation = gl.getAttribLocation(program, "a_position");
      var positionBuffer = gl.createBuffer();
      gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

      var one = (2 * (mouseX / pageX)) - 1;
      var two = (2 * (1 - (mouseY / pageY))) - 1;
      var positions = [
         one, two,
         0, 0.5,
         0.7, 0,
      ];
      gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

      //function resizeCanvasToDisplaySize(canvas, multiplier) {
      //   multiplier = multiplier || 1;
      //   var width  = canvas.clientWidth  * multiplier | 0;
      //   var height = canvas.clientHeight * multiplier | 0;
      //   if (canvas.width !== width ||  canvas.height !== height) {
      //      canvas.width  = width;
      //      canvas.height = height;
      //      return true;
      //   }
      //   return false;
      //}
      //resizeCanvasToDisplaySize(gl.canvas);
      //gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

      gl.clearColor(0, 0, 0, 0);
      gl.clear(gl.COLOR_BUFFER_BIT);

      gl.useProgram(program);

      gl.enableVertexAttribArray(positionAttributeLocation);

      gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

      var size = 2;          // 2 components per iteration
      var type = gl.FLOAT;   // the data is 32bit floats
      var normalize = false; // don't normalize the data
      var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
      var offset = 0;        // start at the beginning of the buffer
      gl.vertexAttribPointer(
         positionAttributeLocation, size, type, normalize, stride, offset);

      var primitiveType = gl.TRIANGLES;
      var offset = 0;
      var count = 3;
      gl.drawArrays(primitiveType, offset, count);

      document.body.style.background = "url(" + canvas.toDataURL() + ")";
   });
}
