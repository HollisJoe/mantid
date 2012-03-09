"""Tests the construction of the various workspace 
property types
"""
import unittest
import testhelpers
from mantid.api import (WorkspaceProperty, MatrixWorkspaceProperty, IEventWorkspaceProperty, 
                        ITableWorkspaceProperty)
from mantid.kernel import Direction

class WorkspacePropertiesTest(unittest.TestCase):

    def _do_test(self, classtype):
       self._do_construction_with_name_default_direction(classtype)
       self._do_construction_with_name_default_direction_optional(classtype)
       
    def _do_construction_with_name_default_direction(self, classtype):
        prop = classtype("NoValidation", "test", Direction.Output)
        self.assertTrue(isinstance(prop, classtype))
        self.assertEquals("NoValidation", prop.name)
        self.assertEquals(Direction.Output, prop.direction)
        self.assertEquals("test", prop.valueAsStr)
        
    def _do_construction_with_name_default_direction_optional(self, classtype):
        prop = classtype("IsOptional", "test", Direction.Output, True)
        self.assertTrue(isinstance(prop, classtype))
        self.assertEquals("IsOptional", prop.name)
        self.assertEquals(Direction.Output, prop.direction)
        self.assertTrue(prop.isOptional())
        self.assertEquals("test", prop.valueAsStr)

    def _do_construction_with_name_default_direction_optional_locking(self, classtype):
        prop = classtype("DoesNotLock", "test", Direction.Output, True, False)
        self.assertTrue(isinstance(prop, classtype))
        self.assertEquals("DoesNotLock", prop.name)
        self.assertEquals(Direction.Output, prop.direction)
        self.assertTrue(prop.isOptional())
        self.assertFalse(prop.isLocking())
        self.assertEquals("test", prop.valueAsStr)
    
    def test_WorkspaceProperty_can_be_instantiated(self):
        self._do_test(WorkspaceProperty)

    def test_MatrixWorkspaceProperty_can_be_instantiated(self):
        self._do_test(MatrixWorkspaceProperty)

    def test_IEventWorkspaceProperty_can_be_instantiated(self):
        self._do_test(IEventWorkspaceProperty)

    def test_ITableWorkspaceProperty_can_be_instantiated(self):
        self._do_test(ITableWorkspaceProperty)

if __name__ == "__main__":
    unittest.main()