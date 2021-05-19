#!/bin/bash

IMG_PATH="automaton.png"

dot -Tpng -o ${IMG_PATH} dot.gv && open ${IMG_PATH}
