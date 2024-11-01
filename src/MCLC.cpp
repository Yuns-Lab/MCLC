//MCLC
extern "C" {
    void deleteString(const char *ptr) {
        delete [] ptr;
    }
}