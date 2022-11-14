// Merging the Array Function
void Merge(char arr[], int low, int mid, int high)
{
    char tmpArr[high - low + 1];
    int pos = 0, lpos = low, rpos = mid + 1;

    while (lpos <= mid && rpos <= high)
        if (arr[lpos] <= arr[rpos])
            tmpArr[pos++] = arr[lpos++];
        else
            tmpArr[pos++] = arr[rpos++];

    while (lpos <= mid)
        tmpArr[pos++] = arr[lpos++];
    while (rpos <= high)
        tmpArr[pos++] = arr[rpos++];

    int iter;
    for (iter = 0; iter < pos; iter++)
        arr[iter + low] = tmpArr[iter];

    return;
}

// Main MergeSort function
void MergeSort(char arr[], int low, int high)
{
    if (low < high)
    {
        int mid = (low + high) / 2;
        MergeSort(arr, low, mid);
        MergeSort(arr, mid + 1, high);

        Merge(arr, low, mid, high);
    }
}