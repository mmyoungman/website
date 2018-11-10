main();
function main() {
  const canvas = document.querySelector("#c");
  const gl = canvas.getContext("webgl");

  if (gl === null) {
    alert("Unable to initialize WebGL!");
    return;
  }

  gl.clearColor(1.0, 1.0, 1.0, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
  document.body.style.background = "url(" + canvas.toDataURL() + ")";
}
