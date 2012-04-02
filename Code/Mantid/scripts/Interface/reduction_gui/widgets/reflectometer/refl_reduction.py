from PyQt4 import QtGui, uic, QtCore
import reduction_gui.widgets.util as util
import math
import os
import time
import sys
from functools import partial
from reduction_gui.reduction.reflectometer.refl_data_script import DataSets as REFLDataSets
from reduction_gui.reduction.reflectometer.refl_data_series import DataSeries
from reduction_gui.settings.application_settings import GeneralSettings
from base_ref_reduction import BaseRefWidget
import ui.reflectometer.ui_data_refl_simple

IS_IN_MANTIDPLOT = False
try:
    import mantidplot
    from MantidFramework import *
    mtd.initialise(False)
    from mantidsimple import *
    import _qti
    from reduction.instruments.reflectometer import data_manipulation

    IS_IN_MANTIDPLOT = True
except:
    pass

class DataReflWidget(BaseRefWidget):    
    """
        Widget that present instrument details to the user
    """
    ## Widget name
    name = "Data"      
    instrument_name = 'REF_L'
    short_name = 'REFL'
    peak_pixel_range = []
    background_pixel_range = []

    def __init__(self, parent=None, state=None, settings=None, name="REFL", data_proxy=None):      
        super(DataReflWidget, self).__init__(parent, state, settings, data_proxy=data_proxy) 

        class SummaryFrame(QtGui.QFrame, ui.reflectometer.ui_data_refl_simple.Ui_Frame):
            def __init__(self, parent=None):
                QtGui.QFrame.__init__(self, parent)
                self.setupUi(self)
                
        self.short_name = name
        self._settings.instrument_name = name
            
        self._summary = SummaryFrame(self)
        self.initialize_content()
        self._layout.addWidget(self._summary)

        if state is not None:
            self.set_state(state)
        else:
            self.set_state(DataSeries(data_class=REFLDataSets))

    def initialize_content(self):
        super(DataReflWidget, self).initialize_content()
        self._summary.q_step_edit.setValidator(QtGui.QDoubleValidator(self._summary.q_step_edit))
        self._summary.instrument_group_box.hide()
        self._ref_instrument_selected()
        
    def _ref_instrument_selected(self):
        self.instrument_name = "REF_L"
        self._summary.center_pix_radio.hide()
        self._summary.center_pix_edit.hide()
        self._summary.angle_radio.hide()
        self._summary.angle_edit.hide()
        self._summary.angle_unit_label.hide()
        self._summary.angle_offset_label.show()
        self._summary.angle_offset_edit.show()
        self._summary.angle_offset_pm_label.show()
        self._summary.angle_offset_error_edit.show()
        self._summary.angle_offset_unit_label.show()
        self._summary.det_angle_offset_check.hide()
        self._summary.det_angle_offset_edit.hide()
        self._summary.det_angle_offset_unit_label.hide()
        self._summary.det_angle_check.hide()
        self._summary.det_angle_edit.hide()
        self._summary.det_angle_unit_label.hide()
        self._summary.direct_pixel_check.hide()
        self._summary.direct_pixel_edit.hide()
        self._summary.q_bins_label.hide()
        self._summary.ref_pix_estimate.hide()
        
        # Output directory
        self._summary.outdir_label.hide()
        self._summary.outdir_edit.hide()
        self._summary.outdir_browse_button.hide()

        #TODO: allow log binning
        self._summary.log_scale_chk.hide()
                 
    def _tof_range_clicked(self, is_checked):
        """
            This is reached by the TOF range switch
        """
        self._summary.tof_min_label.setEnabled(is_checked)
        self._summary.data_from_tof.setEnabled(is_checked)
        self._summary.tof_min_label2.setEnabled(is_checked)
        self._summary.tof_max_label.setEnabled(is_checked)
        self._summary.data_to_tof.setEnabled(is_checked)
        self._summary.tof_max_label2.setEnabled(is_checked)
        
        self._edit_event(None, self._summary.tof_range_switch)

    def set_state(self, state):
        """
            Populate the UI elements with the data from the given state. 
            @param state: data object    
        """
        super(DataReflWidget, self).set_state(state)
        
        if len(state.data_sets)>0:
            self._summary.q_step_edit.setText(str(math.fabs(state.data_sets[0].q_step)))
            
        self._reset_warnings()

    def set_editing_state(self, state):
        super(DataReflWidget, self).set_editing_state(state)
                
        self._summary.tof_range_switch.setChecked(state.crop_TOF_range)
        self._tof_range_clicked(state.crop_TOF_range)
                                                
    def get_state(self):
        """
            Returns an object with the state of the interface
        """
        m = self.get_editing_state()
        state = DataSeries(data_class=REFLDataSets)
        state_list = []
        
        # Common Q binning
        q_min = float(self._summary.q_min_edit.text())
        q_step = float(self._summary.q_step_edit.text())
        if self._summary.log_scale_chk.isChecked():
            q_step = -q_step
            
        # Angle offset
        if hasattr(m, "angle_offset"):
            angle_offset = float(self._summary.angle_offset_edit.text())
            angle_offset_error = float(self._summary.angle_offset_error_edit.text())
                
        for i in range(self._summary.angle_list.count()):
            data = self._summary.angle_list.item(i).data(QtCore.Qt.UserRole).toPyObject()
            # Over-write Q binning with common binning
            data.q_min = q_min
            data.q_step = q_step
        
            # Over-write angle offset
            if hasattr(data, "angle_offset"):
                data.angle_offset = angle_offset
                data.angle_offset_error = angle_offset_error

            state_list.append(data)
        state.data_sets = state_list
        
        return state
    
    def get_editing_state(self):
        m = REFLDataSets()
        
        #Peak from/to pixels
        m.DataPeakPixels = [int(self._summary.data_peak_from_pixel.text()),
                            int(self._summary.data_peak_to_pixel.text())] 
        
        m.data_x_range = [int(self._summary.x_min_edit.text()),
                     int(self._summary.x_max_edit.text())]
        m.data_x_range_flag = self._summary.data_low_res_range_switch.isChecked()
        
        m.norm_x_range = [int(self._summary.norm_x_min_edit.text()),
                          int(self._summary.norm_x_max_edit.text())]
        m.norm_x_range_flag = self._summary.norm_low_res_range_switch.isChecked()
        
        #Background flag
        m.DataBackgroundFlag = self._summary.data_background_switch.isChecked()

        #Background from/to pixels
        roi1_from = int(self._summary.data_background_from_pixel1.text())
        roi1_to = int(self._summary.data_background_to_pixel1.text())
        m.DataBackgroundRoi = [roi1_from, roi1_to, 0, 0]

        #from TOF and to TOF
        from_tof = float(self._summary.data_from_tof.text())
        to_tof = float(self._summary.data_to_tof.text())
        m.DataTofRange = [from_tof, to_tof]
        m.crop_TOF_range = self._summary.tof_range_switch.isChecked()
    
        datafiles = str(self._summary.data_run_number_edit.text()).split(',')
        m.data_files = [int(i) for i in datafiles]
    
        # Normalization flag
        m.NormFlag = self._summary.norm_switch.isChecked()

        # Normalization options
        m.norm_file = int(self._summary.norm_run_number_edit.text())
        m.NormPeakPixels = [int(self._summary.norm_peak_from_pixel.text()),
                            int(self._summary.norm_peak_to_pixel.text())]   
        
        #Background flag
        m.NormBackgroundFlag = self._summary.norm_background_switch.isChecked()

        #Background from/to pixels
        roi1_from = int(self._summary.norm_background_from_pixel1.text())
        roi1_to = int(self._summary.norm_background_to_pixel1.text())
        m.NormBackgroundRoi = [roi1_from, roi1_to]
        
        # Scattering angle
        m.theta = float(self._summary.angle_edit.text())
        m.use_center_pixel = False

        return m