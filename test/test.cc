
#include "test.h"

#include "system.h"

int main(int argc, char* argv[])
{
    wxccErr = stdout;

    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
