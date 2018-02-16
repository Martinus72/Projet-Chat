#ifndef extension
#define extension

/* Extract sub string */
char *str_sub (const char *s, unsigned int start, unsigned int end)
{
   char *new_s = NULL;

   if (s != NULL && start < end)
   {
      new_s = malloc (sizeof (*new_s) * (end - start + 2));
      if (new_s != NULL)
      {
         int i;

         for (i = start; i <= end; i++)
         {
            new_s[i-start] = s[i];
         }
         new_s[i-start] = '\0';
      }
      else
      {
         fprintf (stderr, "Memoire insuffisante\n");
         exit (EXIT_FAILURE);
      }
   }
   return new_s;
}

/* Return first occurence index */
int str_istr (const char *cs, const char *ct)
{
   int index = -1;

   if (cs != NULL && ct != NULL)
   {
      char *ptr_pos = NULL;

      ptr_pos = strstr (cs, ct);
      if (ptr_pos != NULL)
      {
         index = ptr_pos - cs;
      }
   }
   return index;
}

#endif