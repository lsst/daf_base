#!/usr/bin/env python

"""Sphinx configurations to build package documentation."""

import os

from documenteer.sphinxconfig.stackconf import build_package_configs

import lsst.daf.base


_g = globals()
_g.update(build_package_configs(
    project_name='daf_base',
    copyright='2016 Association of Universities for '
              'Research in Astronomy, Inc.',
    version=lsst.daf.base.version.__version__,
    doxygen_xml_dirname=os.path.join(os.path.dirname(__file__), 'xml')))

# DEBUG only
automodsumm_writereprocessed = True
