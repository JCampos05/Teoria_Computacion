#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <iomanip>

class CPPKeywordsMap {
private:
    // Hash set para búsqueda rápida O(1)
    std::unordered_set<std::string> keywordSet;
    
    // Hash map con información adicional sobre cada palabra reservada
    std::unordered_map<std::string, std::pair<std::string, std::string>> keywordInfo;

public:
    CPPKeywordsMap() {
        initializeKeywords();
    }
    
    void initializeKeywords() {
        // Estructura: {palabra, {categoría, versión}}
        std::vector<std::tuple<std::string, std::string, std::string>> keywords = {
            // C++98/03 Keywords
            {"auto", "Storage", "C++98"},
            {"break", "Control", "C++98"},
            {"case", "Control", "C++98"},
            {"catch", "Exception", "C++98"},
            {"char", "Type", "C++98"},
            {"class", "OOP", "C++98"},
            {"const", "Qualifier", "C++98"},
            {"continue", "Control", "C++98"},
            {"default", "Control", "C++98"},
            {"delete", "Memory", "C++98"},
            {"do", "Control", "C++98"},
            {"double", "Type", "C++98"},
            {"else", "Control", "C++98"},
            {"enum", "Type", "C++98"},
            {"extern", "Storage", "C++98"},
            {"false", "Literal", "C++98"},
            {"float", "Type", "C++98"},
            {"for", "Control", "C++98"},
            {"friend", "Access", "C++98"},
            {"goto", "Control", "C++98"},
            {"if", "Control", "C++98"},
            {"inline", "Function", "C++98"},
            {"int", "Type", "C++98"},
            {"long", "Type", "C++98"},
            {"mutable", "Qualifier", "C++98"},
            {"namespace", "Scope", "C++98"},
            {"new", "Memory", "C++98"},
            {"operator", "Overload", "C++98"},
            {"private", "Access", "C++98"},
            {"protected", "Access", "C++98"},
            {"public", "Access", "C++98"},
            {"register", "Storage", "C++98"},
            {"return", "Control", "C++98"},
            {"short", "Type", "C++98"},
            {"signed", "Type", "C++98"},
            {"sizeof", "Operator", "C++98"},
            {"static", "Storage", "C++98"},
            {"struct", "Type", "C++98"},
            {"switch", "Control", "C++98"},
            {"template", "Generic", "C++98"},
            {"this", "Pointer", "C++98"},
            {"throw", "Exception", "C++98"},
            {"true", "Literal", "C++98"},
            {"try", "Exception", "C++98"},
            {"typedef", "Type", "C++98"},
            {"typeid", "RTTI", "C++98"},
            {"typename", "Generic", "C++98"},
            {"union", "Type", "C++98"},
            {"unsigned", "Type", "C++98"},
            {"using", "Scope", "C++98"},
            {"virtual", "OOP", "C++98"},
            {"void", "Type", "C++98"},
            {"volatile", "Qualifier", "C++98"},
            {"wchar_t", "Type", "C++98"},
            {"while", "Control", "C++98"},
            
            // Cast Operators
            {"const_cast", "Cast", "C++98"},
            {"dynamic_cast", "Cast", "C++98"},
            {"reinterpret_cast", "Cast", "C++98"},
            {"static_cast", "Cast", "C++98"},
            
            // Alternative Operators
            {"and", "Operator", "C++98"},
            {"and_eq", "Operator", "C++98"},
            {"bitand", "Operator", "C++98"},
            {"bitor", "Operator", "C++98"},
            {"compl", "Operator", "C++98"},
            {"not", "Operator", "C++98"},
            {"not_eq", "Operator", "C++98"},
            {"or", "Operator", "C++98"},
            {"or_eq", "Operator", "C++98"},
            {"xor", "Operator", "C++98"},
            {"xor_eq", "Operator", "C++98"},
            
            // C++11 Keywords
            {"alignas", "Memory", "C++11"},
            {"alignof", "Memory", "C++11"},
            {"char16_t", "Type", "C++11"},
            {"char32_t", "Type", "C++11"},
            {"constexpr", "Qualifier", "C++11"},
            {"decltype", "Type", "C++11"},
            {"final", "OOP", "C++11"},
            {"noexcept", "Exception", "C++11"},
            {"nullptr", "Pointer", "C++11"},
            {"override", "OOP", "C++11"},
            {"static_assert", "Debug", "C++11"},
            {"thread_local", "Storage", "C++11"},
            
            // C++20 Keywords
            {"concept", "Generic", "C++20"},
            {"requires", "Generic", "C++20"},
            {"co_await", "Coroutine", "C++20"},
            {"co_return", "Coroutine", "C++20"},
            {"co_yield", "Coroutine", "C++20"},
            {"export", "Module", "C++20"},
            {"import", "Module", "C++20"},
            {"module", "Module", "C++20"},
            
            // Assembly
            {"asm", "Assembly", "C++98"},
            
            // Boolean type
            {"bool", "Type", "C++98"}
        };
        
        // Llenar los contenedores
        for (const auto& [keyword, category, version] : keywords) {
            keywordSet.insert(keyword);
            keywordInfo[keyword] = {category, version};
        }
    }
    
    // Función principal para verificar si es palabra reservada
    bool isKeyword(const std::string& word) const {
        return keywordSet.find(word) != keywordSet.end();
    }
    
    // Obtener información sobre una palabra reservada
    std::pair<std::string, std::string> getKeywordInfo(const std::string& word) const {
        auto it = keywordInfo.find(word);
        if (it != keywordInfo.end()) {
            return it->second;
        }
        return {"", ""};
    }
    
    // Obtener todas las palabras reservadas como vector
    std::vector<std::string> getAllKeywords() const {
        std::vector<std::string> result;
        for (const auto& keyword : keywordSet) {
            result.push_back(keyword);
        }
        return result;
    }
    
    // Obtener palabras reservadas por categoría
    std::vector<std::string> getKeywordsByCategory(const std::string& category) const {
        std::vector<std::string> result;
        for (const auto& [keyword, info] : keywordInfo) {
            if (info.first == category) {
                result.push_back(keyword);
            }
        }
        return result;
    }
    
    // Obtener palabras reservadas por versión
    std::vector<std::string> getKeywordsByVersion(const std::string& version) const {
        std::vector<std::string> result;
        for (const auto& [keyword, info] : keywordInfo) {
            if (info.second == version) {
                result.push_back(keyword);
            }
        }
        return result;
    }
    
    // Obtener estadísticas
    void printStatistics() const {
        std::cout << "=== ESTADÍSTICAS DE PALABRAS RESERVADAS ===" << std::endl;
        std::cout << "Total de palabras reservadas: " << keywordSet.size() << std::endl;
        
        // Contar por versión
        std::unordered_map<std::string, int> versionCount;
        std::unordered_map<std::string, int> categoryCount;
        
        for (const auto& [keyword, info] : keywordInfo) {
            versionCount[info.second]++;
            categoryCount[info.first]++;
        }
        
        std::cout << "\nPor versión:" << std::endl;
        for (const auto& [version, count] : versionCount) {
            std::cout << "  " << std::setw(8) << version << ": " << count << " palabras" << std::endl;
        }
        
        std::cout << "\nPor categoría:" << std::endl;
        for (const auto& [category, count] : categoryCount) {
            std::cout << "  " << std::setw(12) << category << ": " << count << " palabras" << std::endl;
        }
    }
    
    // Imprimir todas las palabras con su información
    void printAllKeywords() const {
        std::cout << "\n=== TODAS LAS PALABRAS RESERVADAS ===" << std::endl;
        std::cout << std::left << std::setw(20) << "Palabra" 
                  << std::setw(15) << "Categoría" 
                  << std::setw(10) << "Versión" << std::endl;
        std::cout << std::string(45, '-') << std::endl;
        
        for (const auto& [keyword, info] : keywordInfo) {
            std::cout << std::left << std::setw(20) << keyword 
                      << std::setw(15) << info.first 
                      << std::setw(10) << info.second << std::endl;
        }
    }
    
    // Función de búsqueda avanzada
    std::vector<std::string> searchKeywords(const std::string& pattern) const {
        std::vector<std::string> result;
        for (const auto& keyword : keywordSet) {
            if (keyword.find(pattern) != std::string::npos) {
                result.push_back(keyword);
            }
        }
        return result;
    }
};

// ==================== FUNCIÓN DE TESTING ====================
void testKeywordChecker() {
    CPPKeywordsMap checker;
    
    // Test cases
    std::vector<std::string> testWords = {
        "int",           // keyword
        "class",         // keyword
        "myVariable",    // not keyword
        "for",           // keyword
        "forEach",       // not keyword
        "nullptr",       // C++11 keyword
        "concept",       // C++20 keyword
        "co_await",      // C++20 keyword
        "notAKeyword"    // not keyword
    };
    
    std::cout << "=== PRUEBAS DE DETECCIÓN ===" << std::endl;
    for (const auto& word : testWords) {
        bool isKw = checker.isKeyword(word);
        std::cout << std::setw(15) << word << ": " << (isKw ? "KEYWORD" : "IDENTIFIER");
        
        if (isKw) {
            auto info = checker.getKeywordInfo(word);
            std::cout << " (" << info.first << ", " << info.second << ")";
        }
        std::cout << std::endl;
    }
}

// ==================== ANALIZADOR SIMPLE ====================
class SimpleAnalyzer {
private:
    CPPKeywordsMap keywordChecker;
    
public:
    void analyzeCode(const std::string& code) {
        std::cout << "\n=== ANÁLISIS DE CÓDIGO ===" << std::endl;
        std::cout << "Código: " << code << std::endl;
        
        std::string word;
        std::vector<std::string> tokens;
        
        // Tokenización simple (solo para demostración)
        for (char c : code) {
            if (isalnum(c) || c == '_') {
                word += c;
            } else {
                if (!word.empty()) {
                    tokens.push_back(word);
                    word.clear();
                }
            }
        }
        if (!word.empty()) {
            tokens.push_back(word);
        }
        
        std::cout << "\nTokens encontrados:" << std::endl;
        for (const auto& token : tokens) {
            bool isKw = keywordChecker.isKeyword(token);
            std::cout << "  " << std::setw(15) << token << ": " 
                      << (isKw ? "KEYWORD" : "IDENTIFIER") << std::endl;
        }
    }
};

// ==================== FUNCIÓN PRINCIPAL ====================
int main() {
    CPPKeywordsMap checker;
    
    // Mostrar estadísticas
    checker.printStatistics();
    
    // Pruebas de detección
    testKeywordChecker();
    
    // Ejemplos de búsqueda
    std::cout << "\n=== BÚSQUEDAS ESPECÍFICAS ===" << std::endl;
    
    // Palabras de C++20
    auto cpp20Keywords = checker.getKeywordsByVersion("C++20");
    std::cout << "\nPalabras reservadas de C++20:" << std::endl;
    for (const auto& kw : cpp20Keywords) {
        std::cout << "  " << kw << std::endl;
    }
    
    // Tipos de datos
    auto typeKeywords = checker.getKeywordsByCategory("Type");
    std::cout << "\nTipos de datos:" << std::endl;
    for (const auto& kw : typeKeywords) {
        std::cout << "  " << kw << std::endl;
    }
    
    // Búsqueda por patrón
    auto constWords = checker.searchKeywords("const");
    std::cout << "\nPalabras que contienen 'const':" << std::endl;
    for (const auto& kw : constWords) {
        std::cout << "  " << kw << std::endl;
    }
    
    // Análisis de código simple
    SimpleAnalyzer analyzer;
    analyzer.analyzeCode("int main() { auto x = 42; return 0; }");
    
    // Ejemplo de uso directo del hash map
    std::cout << "\n=== USO DIRECTO ===" << std::endl;
    std::cout << "¿'class' es keyword? " << (checker.isKeyword("class") ? "SÍ" : "NO") << std::endl;
    std::cout << "¿'myVar' es keyword? " << (checker.isKeyword("myVar") ? "SÍ" : "NO") << std::endl;
    
    return 0;
}  