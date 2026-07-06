# tsf-sip-ts

A Test Environment suite that exercises
[tsf-sip](https://github.com/interpretica-io/tsf-sip) (`tapi_sip`)
against a SIP UA/proxy from the agent it runs on — SIP signalling over
libeXosip2, no media.

| Test | What it checks |
|---|---|
| `probe` | `tapi_sip_options()` reaches the configured SIP target and the response is inspected (status, Server/User-Agent/Allow banners) |
| `audit` | `tapi_sip_audit()` produces a well-formed posture report and the gate fails on any finding ≥ HIGH (an accepted anonymous REGISTER being the one that would) |

**The target is a test parameter** (`target`, e.g. `sip:pbx.example`;
optional `from`). With none set, both tests **skip cleanly** — this
suite ships without a PBX to point at, so a bare run is a skip, not a
failure. The unauthenticated-REGISTER and user-enumeration checks write
to a real registrar and run only when `attempt_register` is given (with
`probe_aor`), against a target you are **authorized** to assess.

## Running it

Needs Docker and `test-environment` as a sibling directory:

```bash
./scripts/run.sh docker guess --cfg=localhost          # skips (no target)
./scripts/run.sh docker guess --cfg=localhost \
    --tester-req=... target=sip:pbx.lan                 # against a UA
```

Native (agent on the host, no container): drop `docker`:
`./scripts/run.sh guess --cfg=localhost ...`.

## Build requirement: libosip2 / libeXosip2

`ta_sip` links **libosip2** and **libeXosip2**, which **are not in
Ubuntu/Debian apt** (no `libexosip2-dev`). The suite's Dockerfile builds
both from source (savannah.gnu.org releases, pinned 5.3.0). For a
**native** build on a host, install them the same way:

```bash
for p in osip/libosip2 exosip/libexosip2; do
  v=5.3.0; n=$(basename $p); \
  curl -fsSLO https://download.savannah.gnu.org/releases/$p-$v.tar.gz && \
  tar xf $n-$v.tar.gz && (cd $n-$v && ./configure --prefix=/usr/local && make && sudo make install); \
done; sudo ldconfig
```

## Status

**Not yet run.** Written alongside tsf-sip but not built or executed
here (no TE toolchain). tsf-sip's libeXosip2 usage was syntax-checked
against the real headers (5.3.0); the TE engine-side C and a live SIP
exchange were not. The first run should expect the ordinary first-build
fixes.
