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


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
