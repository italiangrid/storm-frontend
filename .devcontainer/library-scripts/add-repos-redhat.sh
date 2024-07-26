#!/usr/bin/env bash
#
# Copyright (c) Istituto Nazionale di Fisica Nucleare
# Licensed under the EUPL
#
# Syntax: ./add-repos-redhat.sh

set -e

repo_list="epel-release"

dnf install -y ${repo_list}

dnf config-manager --set-enabled crb
