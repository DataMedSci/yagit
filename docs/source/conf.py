# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
project = "Yet Another Gamma Index Tool"
copyright = "2023, DataMedSci"
author = "DataMedSci"


# -- General configuration ---------------------------------------------------
extensions = ["breathe"]

templates_path = ["_templates"]
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
html_theme = "renku"
html_static_path = ["_static"]


# -- Breathe configuration ---------------------------------------------------
breathe_projects = {"yagit": "../xml"}
breathe_default_project = "yagit"
breathe_default_members = ("members", "private-members", "undoc-members")
breathe_show_include = False

# breathe_debug_trace_directives = True
# breathe_debug_trace_doxygen_ids = True
# breathe_debug_trace_qualification = True
