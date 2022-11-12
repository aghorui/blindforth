/**
 *
 * graphviz.hpp - graphviz dotfile generation convenience functions
 *
 * Copyright (c) 2022 Anamitra Ghorui
 *
 * LICENSE_HERE
 *
 */

#include <stdio.h>

struct Graphviz {
	FILE *f;

	Graphviz(FILE *fp) {
		f = fp;
	}

	static inline void start() {

	}

	static inline void end() {

	}

	static inline void accept() {

	}

	static inline void trap() {

	}

	static inline void edge(char start, char end, char label) {

	}
};