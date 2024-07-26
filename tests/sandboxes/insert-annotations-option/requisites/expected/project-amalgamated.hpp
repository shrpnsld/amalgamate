#pragma once

// ============================================================================
// == ./implementation.hpp ==
// ==
// #pragma once
	// ============================================================================
	// == ./utility.hpp ==
	// ==
	// #pragma once
	void utility()
	{
	}
	// ==
	// == ./utility.hpp ==
	// ============================================================================


void func()
{
	utility()
}
// ==
// == ./implementation.hpp ==
// ============================================================================

// ============================================================================
// == ./interface.hpp ==
// ==
// #pragma once
// #include "implementation.hpp"    // amalgamate: file already inlined
// #include "utility.hpp"    // amalgamate: file already inlined

void inteface()
{
	implementation()
	utility()
}
// ==
// == ./interface.hpp ==
// ============================================================================

