/*****   macros create functional code   *****/
#define pivot_index() (begin+(end-begin)/2)
#define swap(a,b,t) ((t)=(a),(a)=(b),(b)=(t))
 
void sort(int array[], int begin, int end) {
   /*** Use of static here will reduce memory footprint, but will make it thread-unsafe ***/
   static int pivot;
   static int t;     /* temporary variable for swap */
   if (end > begin) {
      int l = begin + 1;
      int r = end;
      swap(array[begin], array[pivot_index()], t); /*** choose arbitrary pivot ***/
      pivot = array[begin];
      while(l < r) {
         if (array[l] <= pivot) {
            l++;
         } else {
            while(l < --r && array[r] >= pivot) /*** skip superfluous swaps ***/
               ;
            swap(array[l], array[r], t); 
         }
      }
      l--;
      swap(array[begin], array[l], t);
      sort(array, begin, l);
      sort(array, r, end);
   }
}
 
#undef swap
#undef pivot_index
