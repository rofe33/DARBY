#pragma once

#include "DARBY_config.h"
#include "DARBY_ref.h"

typedef struct {
    int start;
    int end;
} DARBY_range;

typedef struct {
    int current;
    int next_match;
    DARBY_range matches[2];
} DARBY_next_data;

int
DARBY_next_verse(const DARBY_ref *ref, const DARBY_config *config, DARBY_next_data *next);
