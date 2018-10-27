#include <Python.h>

// Function 1: A simple 'hello world' function
static PyObject* helloworld(PyObject* self, PyObject* args) {   
    printf("Hello World\n");
    return Py_None;
}

static PyObject* say_hello(PyObject* self, PyObject* args) {
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition 
static PyMethodDef myMethods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },
    { NULL, NULL, 0, NULL }
};

// Our Module Definition struct
static struct PyModuleDef postToHtml = {
    PyModuleDef_HEAD_INIT,
    "postToHtml",
    "Post To HTML Module",
    -1,
    myMethods
};

// Initializes our module using our above struct
PyMODINIT_FUNC PyInit_postToHtml(void) {
    return PyModule_Create(&postToHtml);
}
