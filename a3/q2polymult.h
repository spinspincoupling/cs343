#pragma once

typedef struct{int *arr; size_t size;} poly_t;

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta );
