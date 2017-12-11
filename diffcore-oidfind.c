/*
 * Copyright (c) 2017 Google Inc.
 */
#include "cache.h"
#include "diff.h"
#include "diffcore.h"

static void diffcore_filter_blobs(struct diff_queue_struct *q,
				  struct diff_options *options)
{
	int src, dst;

	if (!options->oidfind)
		BUG("oidfind oidset not initialized???");

	for (src = dst = 0; src < q->nr; src++) {
		struct diff_filepair *p = q->queue[src];

		if (!DIFF_PAIR_UNMERGED(p) &&
		    ((DIFF_FILE_VALID(p->one) &&
		     oidset_contains(options->oidfind, &p->one->oid)) ||
		    (DIFF_FILE_VALID(p->two) &&
		     oidset_contains(options->oidfind, &p->two->oid)))) {
			q->queue[dst] = p;
			dst++;
		} else {
			diff_free_filepair(p);
		}
	}

	if (!dst) {
		free(q->queue);
		DIFF_QUEUE_CLEAR(q);
	} else {
		q->nr = dst;
	}
}

void diffcore_oidfind(struct diff_options *options)
{
	diffcore_filter_blobs(&diff_queued_diff, options);
}
