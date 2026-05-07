#!/bin/bash

read -r YEAR MONTH DAY HOUR MINUTE <<< "$(TZ='Europe/Copenhagen' date +'%Y %m %d %H %M')"

# Note: This is going to collapse on 2080-08-01
EPOCH_YEAR=2026

# Note: The "10#" forces bash to treat the numbers as Base-10.
# Without this, bash treats numbers with leading zeros (like "08" or "09")
# as octal and will throw a syntax error.
MONTHS_SINCE_EPOCH=$(( (YEAR - EPOCH_YEAR) * 12 + 10#$MONTH ))
MAJOR=$(( MONTHS_SINCE_EPOCH * 100 + 10#$DAY ))
MINOR=$(( 10#$HOUR ))
PATCH=$(( 10#$MINUTE ))

# Output the formatted version number
# %04d = 4-digit zero-padded number (Major/MonthsSinceEpochAndDay)
# %02d = 2-digit zero-padded number (Minor/Hours and Patch/Minutes)
printf "%04d.%02d.%02d\n" "$MAJOR" "$MINOR" "$PATCH"