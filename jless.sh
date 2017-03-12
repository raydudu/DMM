#!/bin/sh

jq 'del(.allocations[].allocations)' $1 | less
