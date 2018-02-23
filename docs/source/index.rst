.. Documentation master file
   It contains a hidden root toctree directive so that Sphinx
   has an internal index of all of the pages and doesn't
   produce a plethora of warnings about most documents not being in
   a toctree.
   See http://sphinx-doc.org/tutorial.html#defining-document-structure

.. _contents:

.. image:: images/Mantid_Logo_Transparent.png
   :alt: The logo for the Mantid Project
   :align: center

====================
Mantid Documentation
====================

.. toctree::
   :hidden:
   :glob:
   :maxdepth: 1

   algorithms/index
   algorithms/*
   concepts/index
   interfaces/index
   fitfunctions/index
   fitminimizers/index
   techniques/index
   api/index
   release/index
   training/index
   references


This is the documentation for Mantid |release|.

**Sections:**

.. image:: images/mantid.png
   :alt: A preying mantis with arms upraised
   :width: 200px
   :align: right

* :ref:`Algorithms List`
* :ref:`concepts contents`
* :ref:`interfaces contents`
* :ref:`Fit Functions List`
* :ref:`fitminimizers contents`
* :ref:`techniques contents`
* :ref:`api`
    - :ref:`Python <pythonapi>`
    - `C++ <http://doxygen.mantidproject.org/>`_ (Doxygen)
* :ref:`release_notes`
* :ref:`references`
