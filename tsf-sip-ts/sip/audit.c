/** @file
 * @brief SIP Group
 *
 * Read a SIP endpoint's security posture with tapi_sip_audit() and gate
 * on it. The target is a test parameter (@c target); with none set the
 * test skips. The unauthenticated-REGISTER / user-enumeration probes
 * write to a real registrar, so they run only when @c attempt_register
 * is given (with @c probe_aor). The gate fails on any finding at least
 * HIGH.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "sip/audit"

#include "te_config.h"
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_cybersec.h"
#include "tapi_sip.h"
#include "tapi_sip_audit.h"
#include "tsapi_sip.h"

int
main(int argc, char **argv)
{
    tsapi_sip_session sess = {0};
    tapi_cybersec_report report;
    tapi_sip_audit_policy policy = tapi_sip_default_audit_policy;
    te_string verdict = TE_STRING_INIT;
    const char *target = NULL;
    const char *probe_aor = NULL;
    te_bool attempt_register = false;
    te_bool report_ready = false;

    TEST_START;
    TEST_GET_OPT_STRING_PARAM(target);
    TEST_GET_OPT_STRING_PARAM(probe_aor);
    TEST_GET_OPT_BOOL_PARAM(attempt_register);

    if (target == NULL || target[0] == '\0')
        TEST_SKIP("No SIP target configured (set the 'target' parameter)");

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_sip_session_init(&sess, "pco_sip_audit"));

    TEST_STEP("Read the SIP posture of %s into a report", target);
    policy.target = target;
    policy.attempt_register = attempt_register;
    policy.probe_aor = probe_aor;

    tapi_cybersec_report_init(&report);
    report_ready = true;
    CHECK_RC(tapi_sip_audit(sess.pco, &policy, &report));
    tapi_cybersec_report_log(&report);

    if (tapi_cybersec_report_count(&report, TAPI_CYBERSEC_SEV_INFO) == 0)
        TEST_VERDICT("the SIP audit produced no findings at all");

    TEST_STEP("Gate: fail on anything at least HIGH");
    if (tapi_cybersec_report_verdict(&report, TAPI_CYBERSEC_SEV_HIGH,
                                     &verdict))
        TEST_VERDICT("%s", verdict.ptr);

    TEST_SUCCESS;

cleanup:
    te_string_free(&verdict);
    if (report_ready)
        tapi_cybersec_report_free(&report);
    tsapi_sip_session_fini(&sess);
    TEST_END;
}
