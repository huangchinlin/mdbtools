/* MDB Tools - A library for reading MS Access database file
 * Copyright (C) 2000 Brian Bruns
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* this utility dumps the schema for an existing database */

#include "mdbtools.h"

main (int argc, char **argv)
{
int   i, j, k;
MdbHandle *mdb;
MdbCatalogEntry entry;
MdbTableDef *table;
MdbColumn *col;

 if (argc < 2) {
   fprintf (stderr, "Usage: schema <file>\n");
   exit (1);
 }
 
 /* open the database */

 mdb = mdb_open (argv[1]);

 /* read the catalog */
 
 mdb_read_catalog (mdb, MDB_TABLE);

 /* loop over each entry in the catalog */

 for (i=0; i < mdb->num_catalog; i++) 
   {
     entry = g_array_index (mdb->catalog, MdbCatalogEntry, i);

     /* if it's a table */

     if (entry.object_type == MDB_TABLE)
       {
	 /* skip the MSys tables */
       if (strncmp (entry.object_name, "MSys", 4))
	 {
	   
	   /* make sure it's a table (may be redundant) */

	   if (!strcmp (mdb_get_objtype_string (entry.object_type), "Table"))
	     {
	       /* drop the table if it exists */
	       fprintf (stdout, "DROP TABLE %s;\n", entry.object_name);

	       /* create the table */
	       fprintf (stdout, "CREATE TABLE %s\n", entry.object_name);
	       fprintf (stdout, " (\n");
	       	       
	       table = mdb_read_table (&entry);

	       /* get the columns */
	       mdb_read_columns (table);

	       /* loop over the columns, dumping the names and types */

	       for (k = 0; k < table->num_cols; k++)
		 {
		   col = g_ptr_array_index (table->columns, k);
		   
		   fprintf (stdout, "\t%s\t\t\t%s", col->name, 
			    mdb_get_coltype_string (col->col_type));
		   
		   if (col->col_size != 0)
		     fprintf (stdout, " (%d)", col->col_size);
		   
		   if (k < table->num_cols - 1)
		     fprintf (stdout, ", \n");
		   else
		     fprintf (stdout, "\n");
		 }

	       fprintf (stdout, "\n);\n");
	       fprintf (stdout, "-- CREATE ANY INDEXES ...\n");
	       fprintf (stdout, "\n");
	     }
	 }
     }
   }
 
 mdb_free_handle (mdb);
}
