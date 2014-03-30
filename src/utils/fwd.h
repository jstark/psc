#ifndef PSC_UTILS_FWD_H_INCLUDED
#define PSC_UTILS_FWD_H_INCLUDED

#define FWD_DECL_NS_NS_CL(ns1, ns2, cl1) \
    namespace ns1 { namespace ns2 { class cl1; } }

#define FWD_DECL_NS_CL(ns1, cl1) \
    namespace ns1 { class cl1; }

#endif
