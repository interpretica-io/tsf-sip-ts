/** @file
 * @brief SIP Group
 *
 * OPTIONS-probe a SIP UA/proxy and inspect what it says about itself.
 * The target is a test parameter (@c target, e.g. "sip:pbx.example");
 * with none set the test skips cleanly - this suite ships without a PBX
 * to point at, so a bare run is a skip, not a failure.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "sip/probe"

#include "te_config.h"
#include "tapi_test.h"

#include "tapi_sip.h"
#include "tsapi_sip.h"

int
main(int argc, char **argv)
{
    tsapi_sip_session sess;
    tapi_sip_probe probe;
    const char *target = NULL;
    const char *from = NULL;
    te_bool probe_ready = false;

    TEST_START;
    TEST_GET_OPT_STRING_PARAM(target);
    TEST_GET_OPT_STRING_PARAM(from);

    if (target == NULL || target[0] == '\0')
        TEST_SKIP("No SIP target configured (set the 'target' parameter)");
    if (from == NULL || from[0] == '\0')
        from = "sip:probe@tsf.invalid";

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_sip_session_init(&sess, "pco_sip_probe"));

    TEST_STEP("OPTIONS %s", target);
    CHECK_RC(tapi_sip_options(sess.pco, target, from, TAPI_SIP_UDP, 5000,
                              &probe));
    probe_ready = true;
    tapi_sip_probe_log(&probe);

    if (probe.status == 0)
        TEST_VERDICT("no SIP UA answered OPTIONS at %s", target);

    RING("SIP UA at %s answered %d (server=%s)", target, probe.status,
         probe.server != NULL ? probe.server : "?");

    TEST_SUCCESS;

cleanup:
    if (probe_ready)
        tapi_sip_probe_free(&probe);
    tsapi_sip_session_fini(&sess);
    TEST_END;
}
