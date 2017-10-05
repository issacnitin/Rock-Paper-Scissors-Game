#include <gtk/gtk.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>

#include <my_global.h>
#include <mysql.h>


GtkTextBuffer *buffer;
int fds[2];
int map[] = {1, 1, 1};
GtkWidget   *view, *entry;

void input_callback( gpointer          data,
                    gint              source,
                    GdkInputCondition condition )
{
   gchar buf[1024];
   gint chars_read;
   GtkTextIter iter;
   chars_read = 1024;
   gtk_text_buffer_get_end_iter(buffer, &iter);
   while (chars_read == 1024){
     chars_read = read(fds[0], buf, 1024);
     // fprintf(stderr, "%i chars: %s\n", chars_read, buf);
     gtk_text_buffer_insert (buffer, &iter, buf, chars_read);
   }
}

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void processes( GtkWidget *widget,
                   gpointer   data )
{
    char command[100];
    sprintf(command, "./processes %s", gtk_entry_get_text(entry));
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    gtk_text_buffer_set_text (buffer, "", -1);
    int winner = system(command);
    MYSQL *con = mysql_init(NULL);
    mysql_real_connect(con, "localhost", "root", "root", "rockpaperscissors", 0, NULL, 0); 
      
    char buffer2[200];
    sprintf(buffer2, "insert into game(player1, player2) values(%d, %d)", winner==256?1:0, winner==512?1:0);
    mysql_query(con, buffer2); 

    mysql_close(con);
}

static void threads(GtkWidget *widget, gpointer data) {
    char command[100];
    sprintf(command, "./threads %s", gtk_entry_get_text(entry));
    
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    gtk_text_buffer_set_text (buffer, "", -1);
    int winner = system(command);
    MYSQL *con = mysql_init(NULL);
    mysql_real_connect(con, "localhost", "root", "root", "rockpaperscissors", 0, NULL, 0); 
      
    char buffer2[200];
    sprintf(buffer2, "insert into game_threads(player1, player2) values(%d, %d)", winner==256?1:0, winner==512?1:0);
    mysql_query(con, buffer2); 
    mysql_close(con);

}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

    return TRUE;
}

/* Another callback */
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int main( int   argc, char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    GtkWidget *button, *button2;
    GtkWidget * box = gtk_vbox_new(0,0);
    GtkWidget *scrolled_window, *label, *table;
    pipe (fds);
    // Redirect fds[1] to be writed with the standard output.
    dup2 (fds[1], STDOUT_FILENO);

    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_container_add(GTK_CONTAINER(window),box);
    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event",
		      G_CALLBACK (destroy), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (box), 10);
    
    /* Creates a new button with the label "Hello World". */
    button = gtk_button_new_with_label ("Run Game with processes");
    button2 = gtk_button_new_with_label ("Run Game with threads");
    
   table = gtk_table_new (1, 2, TRUE);
   gtk_container_add (GTK_CONTAINER (box), table);
    label = gtk_label_new ("Points to be won" );

    gtk_table_set_homogeneous(GTK_TABLE (table), TRUE);
    gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 0, 1);


    //create a text box
    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry),0);
    gtk_table_attach_defaults (GTK_TABLE (table), entry, 0, 1, 0, 1);

    /* When the button receives the "clicked" signal, it will call the
     * function hello() passing it NULL as its argument.  The hello()
     * function is defined above. */
    g_signal_connect (button, "clicked",
		      G_CALLBACK (processes), NULL);
    
    g_signal_connect(button2, "clicked", G_CALLBACK(threads), NULL);


    view = gtk_text_view_new ();
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    gtk_text_buffer_set_text (buffer, "", -1);
    gdk_input_add(fds[0], GDK_INPUT_READ, input_callback, NULL);

    /* This packs the button into the window (a gtk container). */
    GtkAdjustment* height = gtk_adjustment_new(200, 100, 500, 10, 10, 10);
    GtkAdjustment* width = gtk_adjustment_new(200, 100, 500, 10, 10, 10);
    
    scrolled_window = gtk_scrolled_window_new (width, height);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
                                  GTK_POLICY_AUTOMATIC, 
                                  GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (box), button);
    gtk_container_add (GTK_CONTAINER (box), button2);
  /* The function directly below is used to add children to the scrolled window 
   * with scrolling capabilities (e.g text_view), otherwise, 
   * gtk_scrolled_window_add_with_viewport() would have been used
   */
  gtk_container_add (GTK_CONTAINER (scrolled_window), 
                                         view);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 5);
 
  
  gtk_container_add (GTK_CONTAINER (box), scrolled_window);
    /* The final step is to display this newly created widget. */
    gtk_widget_show (button);
    gtk_widget_show (button2);
    gtk_widget_show (scrolled_window);
    gtk_widget_show (view);
    gtk_widget_show (table);
    gtk_widget_show (label);
    gtk_widget_show (entry);
          
    gtk_window_set_default_size(GTK_WINDOW(window),500, 600); 
    /* and the window */
    gtk_widget_show (window);
    gtk_widget_show (box);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();
    
    return 0;
}