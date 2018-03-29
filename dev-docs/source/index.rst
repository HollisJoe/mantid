.. _contents:

=======================
Developer Documentation
=======================

These pages contain the developer documentation for mantid. They are aimed at those who are modifying the
source code of the project. For user documentation please see :ref:`here <mantid:contents>`.

===============
Getting Started
===============

.. toctree::
   :hidden:

   DeveloperAccounts
   GettingStarted

:doc:`DeveloperAccounts`
   Details of the accounts required for developers.

:doc:`GettingStarted`
   Describes the process of obtaining and building the mantid code base.

=======
Testing
=======

.. toctree::
   :hidden:

   RunningTheUnitTests
   UnitTestGoodPractice
   WritingPerformanceTests
   DataFilesForTesting

:doc:`RunningTheUnitTests`
   Details on how to run the suite of unit tests

:doc:`UnitTestGoodPractice`
   Guidance on writing good unit tests

:doc:`WritingPerformanceTests`
   A walk through of how to write a performance test

:doc:`SystemTests`
   Guidance on working with the system tests

:doc:`DataFilesForTesting`
   How to work with test data files in the mantid repository

===============
GUI Development
===============

.. toctree::
   :hidden:

   GUIDesignGuidelines
   MVPTutorial/index

:doc:`GUIDesignGuidelines`
   Gives some guidelines to consider when developing a new graphical user interface.

:doc:`MVP Tutorial <MVPTutorial/index>`
   A hands-on tutorial on how to implement a user GUI using the Model-View-Presenter (MVP) pattern.


===================
Component Overviews
===================

.. toctree::
   :maxdepth: 1

   AlgorithmMPISupport
   IndexProperty
   ISISSANSReductionBackend
   LoadAlgorithmHook
   Logging
   RemoteJobSubmissionAPI
   WritingAnAlgorithm
   WritingCustomConvertToMDTransformation
