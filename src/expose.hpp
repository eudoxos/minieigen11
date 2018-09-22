// functions defined in the respective .cpp files
void expose_matrices(py::module&);
void expose_vectors(py::module&);
void expose_boxes(py::module&);
void expose_quaternion(py::module&);
void expose_complex(py::module&); // does nothing if _COMPLEX_SUPPORT is not #defined
void expose_converters(py::module&);
