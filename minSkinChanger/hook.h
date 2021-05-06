#pragma once

void Patch(BYTE* src, BYTE* dst, unsigned int len);

bool Detour(BYTE* src, BYTE* dst, int len);

BYTE* TrampHook(BYTE* src, BYTE* dst, int len);