#include "bbox.h"

void bbox_t::read(reader_t *r)
{
	r->read_float(&x1);
	r->read_float(&y1);
	r->read_float(&z1);

	r->read_float(&x2);
	r->read_float(&y2);
	r->read_float(&z2);
}
