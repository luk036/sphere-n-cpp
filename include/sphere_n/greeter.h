#pragma once

#include <string>

namespace sphere_n {

/**  Language codes to be used with the SphereN class */
enum class LanguageCode { EN, DE, ES, FR };

/**
 * @brief A class for saying hello in multiple languages
 */
class Sphere_N {
  std::string name;

public:
  /**
   * @brief Creates a new xn
   * @param name the name to greet
   */
  Sphere_N(std::string name);

  /**
   * @brief Creates a localized string containing the greeting
   * @param lang the language to greet in
   * @return a string containing the greeting
   */
  std::string greet(LanguageCode lang = LanguageCode::EN) const;
};

} // namespace sphere_n
