#pragma once

// not entirely sure what this file is going to be.
// maybe some utility macros
// i kind of want to learn more about useful macros so here we are...

// TODO: check if x is acutally a ptr??
#define valid_ptr(x)																	\
	if(!(x))																			\
	{																					\
		log(Severity::ERROR, "ptr invalid file: {}, line: {}", __FILE__, __LINE__);		\
		return;																			\
	}
