To make the docs for daf_base alone:

```
sphinx-build -b html -n -d _build/doctrees  . _build/html
```

*Note:* I'm have difficulty running from the Makefile:

```
Exception occurred:
  File "/Users/jsick/lsst/_DM-6199-pipelines-docs/lsstsw/miniconda/lib/python2.7/importlib/__init__.py", line 37, in import_module
      __import__(name)
      ImportError: No module named _baseLib
```
