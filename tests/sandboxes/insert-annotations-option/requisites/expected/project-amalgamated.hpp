#pragma once

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

// ============================================================================
// == ./interface.hpp ==
// ==
// #pragma once
	// ============================================================================
	// == ./implementation.hpp ==
	// ==
	// #pragma once
	// #include "utility.hpp"    // amalgamate: file already inlined

	void func()
	{
		utility()
	}
	// ==
	// == ./implementation.hpp ==
	// ============================================================================

// #include "utility.hpp"    // amalgamate: file already inlined

void inteface()
{
	implementation()
	utility()
}
// ==
// == ./interface.hpp ==
// ============================================================================

