#include <parcel.h>

/* Constants for response types */
#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1

typedef bool (*ctor_t)(Parcel&);
typedef bool (*verif_t)(int token, Parcel&);
typedef bool (*unsol_verif)(Parcel&);

bool verify_unsolicited(Parcel& p);
void print_usage(const char* prog);
struct ril_test_case* get_test_case(int id);
int case_num(void);

struct ril_test_case {
    int id;
    const char* case_name;
    int reqnum;
    ctor_t ctor;
    verif_t verif;
};

struct unsol_test_case {
    int unsol_ril_req;
    unsol_verif uverif;
};