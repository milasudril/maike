#!/bin/bash

export orig_version="2.0.0-beta-5"
export install_dir=$(mktemp -d)
trap "rm -rf $install_dir" EXIT
bootsrap_from_version="2.0.0-beta-5" integration_test/bootstrap_from_release.sh

