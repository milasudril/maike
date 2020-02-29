#!/bin/bash

tidydox
cp -pu __doc/html/* gh-pages
rm -r __doc
