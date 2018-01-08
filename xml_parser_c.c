/************************************************
 *  XML_PARSER
 *  xml_parser_c.c
 *
 *  This version of the XML Parser is currently
 *  a working version. Presently it takes 3 arguments:
 *  a filename, an XPath expression, and a new value.
 *
 *  It search and replace in the node/nodes the new value
 *  and print the output. It will need to be heavily modified
 *  to suit the needs of a stream interstitial program 
 *  that modifies stream log files to maintain their
 *  compatibility on both ends. The requirements of this
 *  will require a separate write-up.
 *
 ************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_SAX1_ENABLED) && \
    defined(LIBXML_OUTPUT_ENABLED)

static void usage(const char *name);
static int  xpath_replace(const char *filename, const xmlChar *xpathExpr, const xmlChar *value);
static void update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar *value);

int main (int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Error: wrong number of arguments.\n");
        usage(argv[0]);
        return -1;
    }

    /* init libxml */
    xmlInitParser();
    LIBXML_TEST_VERSION

    if (xpath_replace(argv[1], BAD_CAST argv[2], BAD_CAST argv[3])) {
        usage(argv[0]);
        return -1;
    }

    /* Shutdown libxml */
    xmlCleanupParser();

    /* mem debug */
    xmlMemoryDump();
    return 0;
}

/**
 *  usage
 *  @name:  the program name
 *
 *  Prints usage info.
 */
static void usage (const char *name)
{
    assert(name);

    fprintf(stderr, "Usage: %s <xml-file> <xpath-expr> <value>\n", name);
}

/**
 *  xpath_replace
 *  @filename:      the input XML filename
 *  @xpathExpr:     the xpath expression for evalutation
 *  @value:         the new node content.
 *
 *  Parse the XML file, evaluate XPath expression and upate the nodes
 *  then print the result.
 *
 *  Returns 0 on success, or a negative integer otherwise.
 */
static int xpath_replace (const char *filename, const xmlChar *xpathExpr, const xmlChar *value)
{
    xmlDocPtr          doc;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr  xpathObj;

    assert(filename);
    assert(xpathExpr);
    assert(value);

    /* Load Document */
    doc = xmlParseFile(filename);
    if (doc == NULL) {
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", filename);
        return -1;
    }

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        fprintf(stderr, "Error: unable to to create new XPath context\n");
        xmlFreeDoc(doc);
        return -1;
    }

    /*  Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr, "Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlFreeDoc(doc);
        return -1;
    }

    /* Update selected nodes */
    update_xpath_nodes(xpathObj->nodesetval, value);

    /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    /* Dump the resulting document */
    xmlDocDump(stdout, doc);

    /* Free the document */
    xmlFreeDoc(doc);

    return 0;
}

/**
 *  update_xpath_nodes:
 *  @nodes:         the nodes set
 *  @value:         the new value for the node(s)
 *
 *  Prints the @nodes content to @output
 *
 */
static void update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar *value)
{
    int size, i;

    assert(value);
    size = (nodes) ? nodes->nodeNr : 0;

    /* Since nodes are processed in reverse order, any processing on the tree structure must be done carefully. */
    for (i = size - 1; i >= 0; i--) {
        assert(nodes->nodeTab[i]);

        xmlNodeSetContent(nodes->nodeTab[i], value);
        if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            nodes->nodeTab[i] = NULL;
    }
}

#else
int main (void)
{
    fprintf(stderr, "XPath support not compiled in\n");
    exit(1);
}
#endif