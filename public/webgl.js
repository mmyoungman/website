'use strict';

main();

function mouseClick(event) {
   var x = event.pageX;
   var y = event.pageY;
   var coords = "X coords: " + x + ", Y coords: " + y;
   document.getElementById("mousePos").innerHTML = coords;
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
   //canvas.width = document.body.clientWidth;
   //canvas.height = document.body.clientHeight;
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

      var positions = [
         0, 0,
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

      // Tell WebGL how to convert from clip space to pixels
      //gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

      // Clear the canvas
      gl.clearColor(0, 0, 0, 0);
      gl.clear(gl.COLOR_BUFFER_BIT);

      // Tell it to use our program (pair of shaders)
      gl.useProgram(program);

      // Turn on the attribute
      gl.enableVertexAttribArray(positionAttributeLocation);

      // Bind the position buffer.
      gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

      // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
      var size = 2;          // 2 components per iteration
      var type = gl.FLOAT;   // the data is 32bit floats
      var normalize = false; // don't normalize the data
      var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
      var offset = 0;        // start at the beginning of the buffer
      gl.vertexAttribPointer(
         positionAttributeLocation, size, type, normalize, stride, offset);

      // draw
      var primitiveType = gl.TRIANGLES;
      var offset = 0;
      var count = 3;
      gl.drawArrays(primitiveType, offset, count);

      document.body.style.background = "url(" + canvas.toDataURL() + ")";
   });

}
