# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui/sans/trans_direct_beam.ui'
#
# Created: Wed Nov 16 13:57:35 2011
#      by: PyQt4 UI code generator 4.7.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_GroupBox(object):
    def setupUi(self, GroupBox):
        GroupBox.setObjectName("GroupBox")
        GroupBox.resize(836, 272)
        self.verticalLayout = QtGui.QVBoxLayout(GroupBox)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setSpacing(6)
        self.gridLayout.setContentsMargins(-1, -1, 0, -1)
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtGui.QLabel(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(205, 0))
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.sample_edit = QtGui.QLineEdit(GroupBox)
        self.sample_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.sample_edit.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.sample_edit.setObjectName("sample_edit")
        self.gridLayout.addWidget(self.sample_edit, 0, 2, 1, 1)
        self.sample_browse = QtGui.QPushButton(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sample_browse.sizePolicy().hasHeightForWidth())
        self.sample_browse.setSizePolicy(sizePolicy)
        self.sample_browse.setToolTip("")
        self.sample_browse.setObjectName("sample_browse")
        self.gridLayout.addWidget(self.sample_browse, 0, 3, 1, 1)
        self.label_2 = QtGui.QLabel(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.direct_edit = QtGui.QLineEdit(GroupBox)
        self.direct_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.direct_edit.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.direct_edit.setObjectName("direct_edit")
        self.gridLayout.addWidget(self.direct_edit, 1, 2, 1, 1)
        self.direct_browse = QtGui.QPushButton(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.direct_browse.sizePolicy().hasHeightForWidth())
        self.direct_browse.setSizePolicy(sizePolicy)
        self.direct_browse.setToolTip("")
        self.direct_browse.setObjectName("direct_browse")
        self.gridLayout.addWidget(self.direct_browse, 1, 3, 1, 1)
        self.beam_radius_edit = QtGui.QLineEdit(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.beam_radius_edit.sizePolicy().hasHeightForWidth())
        self.beam_radius_edit.setSizePolicy(sizePolicy)
        self.beam_radius_edit.setMaximumSize(QtCore.QSize(97, 16777215))
        self.beam_radius_edit.setObjectName("beam_radius_edit")
        self.gridLayout.addWidget(self.beam_radius_edit, 2, 2, 1, 1)
        self.label_3 = QtGui.QLabel(GroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        spacerItem = QtGui.QSpacerItem(22, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem, 0, 5, 1, 1)
        spacerItem1 = QtGui.QSpacerItem(20, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem1, 0, 1, 1, 1)
        self.sample_plot = QtGui.QPushButton(GroupBox)
        self.sample_plot.setMinimumSize(QtCore.QSize(0, 0))
        self.sample_plot.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.sample_plot.setObjectName("sample_plot")
        self.gridLayout.addWidget(self.sample_plot, 0, 4, 1, 1)
        self.direct_plot = QtGui.QPushButton(GroupBox)
        self.direct_plot.setMinimumSize(QtCore.QSize(0, 0))
        self.direct_plot.setMaximumSize(QtCore.QSize(85, 27))
        self.direct_plot.setObjectName("direct_plot")
        self.gridLayout.addWidget(self.direct_plot, 1, 4, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem2 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem2)

        self.retranslateUi(GroupBox)
        QtCore.QMetaObject.connectSlotsByName(GroupBox)

    def retranslateUi(self, GroupBox):
        GroupBox.setWindowTitle(QtGui.QApplication.translate("GroupBox", "GroupBox", None, QtGui.QApplication.UnicodeUTF8))
        GroupBox.setTitle(QtGui.QApplication.translate("GroupBox", "Direct Beam", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("GroupBox", "Sample direct beam data file:", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_edit.setToolTip(QtGui.QApplication.translate("GroupBox", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_browse.setText(QtGui.QApplication.translate("GroupBox", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("GroupBox", "Empty direct beam data file:", None, QtGui.QApplication.UnicodeUTF8))
        self.direct_edit.setToolTip(QtGui.QApplication.translate("GroupBox", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.direct_browse.setText(QtGui.QApplication.translate("GroupBox", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.beam_radius_edit.setToolTip(QtGui.QApplication.translate("GroupBox", "Radius of the beam in pixels.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("GroupBox", "Beam radius (pixels)", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_plot.setToolTip(QtGui.QApplication.translate("GroupBox", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_plot.setText(QtGui.QApplication.translate("GroupBox", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.direct_plot.setToolTip(QtGui.QApplication.translate("GroupBox", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.direct_plot.setText(QtGui.QApplication.translate("GroupBox", "Plot", None, QtGui.QApplication.UnicodeUTF8))

