typedef struct Bucket
{
    int count;
    int size;
    double *b;
} bucket;

typedef struct Bucketbucket
{
    double load;
    bucket *buckets;
    int bucket_count;
} bucketbucket;
