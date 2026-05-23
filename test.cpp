#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>

// A node structure for the General Tree
struct TreeNode {
    std::string tag;
    std::string text_content;
    std::vector<TreeNode*> children;

    TreeNode(const std::string& t) : tag(t) {}
    
    // Destructor to recursively free memory
    ~TreeNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

// Helper function to trim whitespace from text nodes
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// Function to generate the tree from an XML string
TreeNode* buildXMLTree(const std::string& xml) {
    // This represents the "black dot" root in your diagram
    TreeNode* root = new TreeNode("ROOT"); 
    std::stack<TreeNode*> s;
    s.push(root);

    size_t i = 0;
    while (i < xml.length()) {
        size_t openBracket = xml.find('<', i);
        if (openBracket == std::string::npos) break;

        // Capture any text content between the previous '>' and current '<'
        std::string text = trim(xml.substr(i, openBracket - i));
        if (!text.empty() && s.top() != root) {
            s.top()->text_content = text;
        }

        size_t closeBracket = xml.find('>', openBracket);
        if (closeBracket == std::string::npos) break;

        std::string tagContent = xml.substr(openBracket + 1, closeBracket - openBracket - 1);

        if (tagContent[0] == '?') {
            // Handle XML declaration (e.g., <?xml version="1.0"?>)
            TreeNode* declNode = new TreeNode("xml");
            // Strip out '?xml ' at the start and '?' at the end
            declNode->text_content = tagContent.substr(5, tagContent.length() - 6); 
            root->children.push_back(declNode);
            
        } else if (tagContent[0] == '/') {
            // Handle closing tag (e.g., </customer>)
            // Pop the stack to return to the parent node
            if (s.size() > 1) {
                s.pop();
            }
        } else {
            // Handle opening tag (e.g., <customer id="55000">)
            TreeNode* newNode = new TreeNode(tagContent);
            s.top()->children.push_back(newNode);
            s.push(newNode);
        }
        
        i = closeBracket + 1;
    }
    return root;
}

// Helper function to print the tree visually
void printTree(TreeNode* node, int depth = 0) {
    if (!node) return;

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    
    // Print a connector for children
    if (depth > 0) std::cout << "|-- ";
    
    std::cout << node->tag;
    if (!node->text_content.empty()) {
        std::cout << " -> " << node->text_content;
    }
    std::cout << "\n";

    for (auto child : node->children) {
        printTree(child, depth + 1);
    }
}

int main() {
    std::string xmlData = 
        "<?xml version=\"1.0\"?>\n"
        "<customers>\n"
        "   <customer id=\"55000\">\n"
        "      <name>Charter Group</name>\n"
        "      <address>\n"
        "         <street>100 Main</street>\n"
        "         <city>Framingham</city>\n"
        "         <state>MA</state>\n"
        "         <zip>01701</zip>\n"
        "      </address>\n"
        "      <address>\n"
        "         <street>720 Prospect</street>\n"
        "         <city>Framingham</city>\n"
        "         <state>MA</state>\n"
        "         <zip>01701</zip>\n"
        "      </address>\n"
        "   </customer>\n"
        "   <customer id=\"75000\">\n"
        "      <name>Travel Co</name>\n"
        "      <address>\n"
        "         <street>120 Ridge</street>\n"
        "         <state>MA</state>\n"
        "         <zip>01760</zip>\n"
        "      </address>\n"
        "   </customer>\n"
        "</customers>";

    // Build the tree
    TreeNode* root = buildXMLTree(xmlData);

    // Display the tree
    std::cout << "Generated General Tree:\n";
    std::cout << "-----------------------\n";
    printTree(root);

    // Clean up memory
    delete root;

    return 0;
}