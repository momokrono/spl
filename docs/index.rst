.. SPL documentation master file, created by
   sphinx-quickstart on Fri Mar 25 19:55:18 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to SPL's documentation!
===============================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

.. doxygenstruct:: spl::graphics::rgba
   :undoc-members:
   :members:


Color API
---------

.. doxygenfunction:: spl::graphics::over_no_gamma(rgba foreground, rgba background)

.. doxygenfunction:: spl::graphics::over(rgba foreground, rgba background)

.. doxygennamespace:: spl::graphics::color

Image
-----
An object of type `spl::graphics::image` abstracts the idea of an image. It consists in an owning
wrapper around a dynamic array of `rgba` equipped with pair of integers representing width and height.

Public interface
^^^^^^^^^^^^^^^^
.. doxygenclass:: spl::graphics::image
    :members:


Viewport
--------
An `spl::graphics::basic_viewport` aims to be a view over a section of an existing `image`.
You can use it just as if it were an `image`.
There are two types of `basic_viewport`. The main one is the ``spl::graphics::basic_viewport<false>``
which represent a view able to modify the underlying image's pixels. The library provides an alias
for it: `spl::graphics::viewport`.
The second kind of basic_viewport is the ``spl::graphics::basic_viewport<true>``, aliased as
`spl::graphics::image_view`, which represent a view unable to modify the underlying image's pixel,
as if it was read-only.

Public interface
^^^^^^^^^^^^^^^^
.. doxygenclass:: spl::graphics::basic_viewport
    :members:


Drawable
--------

.. doxygenconcept:: spl::drawable


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
