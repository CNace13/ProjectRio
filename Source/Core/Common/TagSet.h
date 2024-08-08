#pragma once

#include <map>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <cstdio>
#include <iostream>

#include "Common/HttpRequest.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Tag
{
    class Tag
    {
    public:
        Tag() {}
        Tag(int in_id, std::string in_name, bool in_active, int in_date_created, std::string in_desc, int in_community_id, std::string in_type, std::optional<std::string> in_gecko_code, std::optional<std::string> in_game):
            id(in_id), 
            name(in_name),
            active(in_active),
            date_created(in_date_created),
            desc(in_desc),
            community_id(in_community_id),
            type(in_type),
            gecko_code(in_gecko_code),
            game(in_game)
            {}
        int id;
        std::string name;
        bool active;
        int date_created;
        std::string desc;
        int community_id;
        std::string type;
        std::optional<std::string> gecko_code;
        std::optional<std::string> game;
    };


    class TagSet
    {
    public:
        TagSet() {}
        TagSet(int in_id, std::string in_name, std::vector<Tag> in_tags, std::optional<std::string> in_game):
            id(in_id),
            name(in_name),
            tags(in_tags),
            game(in_game)
            {}

        int id;
        std::string name;
        std::vector<Tag> tags;
        std::optional<std::string> game;

        std::vector<std::string> tag_names_vector()
        {
            std::vector<std::string> names;
            for (Tag tag : tags) {
                names.push_back(tag.name);
            }
            return names;
        }

        std::vector<int> tag_ids_vector()
        {
            std::vector<int> ids;
            for (Tag tag : tags) {
                ids.push_back(tag.id);
            }
            return ids;
        }

        std::string tag_names_string()
        {
            std::stringstream sstm;
            sstm << "[";
            for (Tag tag: tags) {
                sstm << "\"" << tag.name << "\", ";
            }
            std::string names = sstm.str();
            // Remove trailing characters ", " and add closing brace
            names.pop_back();
            names.pop_back();
            names += "]";
            return names;
        }

        std::string tag_ids_string()
        {
            std::stringstream sstm;
            sstm << "[";
            for (Tag tag: tags) {
                sstm << tag.id << ", ";
            }
            std::string ids = sstm.str();
            // Remove trailing characters ", " and add closing brace
            ids.pop_back();
            ids.pop_back();
            ids += "]";
            return ids;
        }

        std::string description()
        {
            std::stringstream sstm;
            for (Tag tag: tags) {
                if (tag.type == "Competition") { sstm << tag.desc; }
            }
            return sstm.str();
        }

        // Create monster code string
        std::optional<std::vector<std::string>> gecko_codes_string() 
        {
            std::vector<std::string> code_segments;
            std::vector<Tag>::const_iterator it;
            
            // iterate throught TagSet's Tags and create a vector of code segments (half a code, a string u32)
            for (it = tags.begin(); it != tags.end(); it++) {
                if (it->gecko_code.has_value()) {
                    //sstm << it->gecko_code.value();
                    std::string full_code = it->gecko_code.value();
                    size_t line_pos = 0;
                    std::string code_line;

                    //First seperate line by line
                    std::string line_delimiter = "\n";
                    while ((line_pos = full_code.find(line_delimiter)) != std::string::npos) {
                        code_line = full_code.substr(0, line_pos);
                        std::cout << code_line << std::endl;
                        full_code.erase(0, line_pos + line_delimiter.length());
                        
                        code_segments.push_back(code_line); //Last half is accessed outside of loop
                    }
                }
            }

            // if the vector is not empty return string of gecko codes
            if (code_segments.size() > 0) {
                return code_segments;
            // if vector is empty, return nullopt
            } else {
                return std::nullopt;
            }
        }
    };


   static std::optional<json> ParseResponse(const std::vector<u8>& response)
    {
        const std::string response_string(reinterpret_cast<const char*>(response.data()), response.size());
        
        if (!json::accept(response_string)) {
            // Unable to parse JSON
            return std::nullopt;
        }
        else {
            json json_obj = json::parse(response_string);
            return json_obj;
        }
    }

    static std::optional<TagSet> convertJsonTagSet(const json& tag_set_json) {
        // Initializing variables needed for constructing TagSet
        int id;
        std::string name;
        std::vector<Tag> tags_vector;
        std::optional<std::string> game = std::nullopt;
    
        if (tag_set_json.contains("id") && tag_set_json["id"].is_number_integer()) {
            id = tag_set_json["id"];
        } else {
            // If ID is not an integer, return an empty TagSet because that is a breaking error
            return std::nullopt;
        }

        if (tag_set_json.contains("name") && tag_set_json["name"].is_string()) {
            name = tag_set_json["name"];
        } else {
            // If Name is not a string, return an empty TagSet because that is a breaking error
            return std::nullopt;
        }

        if (tag_set_json.contains("game") && tag_set_json["game"].is_string())
        {
            game = tag_set_json["game"];
        }
        else
        {
            // If Name is not a string, return an empty TagSet because that is a breaking error
            game = std::nullopt;
        }

        // Get a vector of json objects for creating Tags
        if (!tag_set_json.contains("tags") || !tag_set_json["tags"].is_array()) {
            return std::nullopt;
        }
        
        const std::vector<json> tags = tag_set_json["tags"];
        // Loop through the vector of json objects, 
        // validate that the json data meets specifications, 
        // and populate the empty tags_vector with Tags
        for (const json& tag : tags) {
            // Initialize variables needed for Tag creation with default values
            // to use in case provided JSON data does not meet validation requirements
            int tag_id;
            std::string tag_name;
            bool active = false;
            int date_created = -1;
            std::string desc = "";
            int community_id = -1;
            std::string type = "";
            std::optional<std::string> gecko_code = std::nullopt;
            std::optional<std::string> tag_game = std::nullopt;

            if (tag.contains("id") && tag["id"].is_number_integer()) {
                tag_id = tag["id"];
            } else {
                // If ID is not an integer, return an empty TagSet because that is a breaking error
                std::cout << "Invalid Tag ID" << "\n";
                return std::nullopt;
            }

            if (tag.contains("name") && tag["name"].is_string()) {
                tag_name = tag["name"];
            } else {
                // If Name is not a string, return an empty TagSet because that is a breaking error
                std::cout << "Invalid Name" << "\n";
                return std::nullopt;
            }

            if (tag.contains("active") && tag["active"].is_boolean()) {
                active = tag["active"];
            }

            if (tag.contains("date_created") && tag["date_created"].is_number_integer()) {
                date_created = tag["date_created"];
            }

            if (tag.contains("desc") && tag["desc"].is_string()) {
                desc = tag["desc"];
            }

            if (tag.contains("comm_id") && tag["comm_id"].is_number_integer()) {
                community_id = tag["comm_id"];
            }

            if (tag.contains("type") && tag["type"].is_string()) {
                type = tag["type"];
            }

            if (tag.contains("gecko_code") && tag["gecko_code"].is_string()) {
                std::string code = tag["gecko_code"];
                if (!code.empty()) {
                    gecko_code = code;
                }
            }

            if (tag.contains("game") && tag["game"].is_string()) {
                game = tag["game"];
            }

            Tag tag_class = Tag{
                tag_id,
                tag_name,
                active,
                date_created,
                desc,
                community_id,
                type,
                gecko_code,
                game
            };
            tags_vector.push_back(tag_class);
        }

        TagSet tag_set = TagSet{
            id,
            name,
            tags_vector,
            game
        };

        return tag_set;
    }

    static inline std::map<int, TagSet> getAvailableTagSets(Common::HttpRequest& http, std::string rio_key, std::string in_game = "") {
        const Common::HttpRequest::Response response = http.Post(
            "https://api.projectrio.app/tag_set/list",
            fmt::format("{{\"Active\":\"true\", \"Rio Key\":\"{}\", \"game\":\"{}\"}}", rio_key, in_game),
            {{"Content-Type", "application/json"}, }
        );

        if (!response) {
            std::cout << "No Response" << "\n";
            std::map<int, TagSet> empty_map;
            return empty_map;
        }

        auto json_obj = ParseResponse(response.value());
        if (!json_obj) {
            std::cout << "No JSON" << "\n";
            std::map<int, TagSet> empty_map;
            return empty_map;
        }

        // Initialize map that will be populated with TagSets and returned at end of function
        std::map<int, TagSet> tag_sets;

        // Create a vector of tag_sets as json objects
        if (!json_obj->contains("Tag Sets") || !(*json_obj)["Tag Sets"].is_array()) {
            return tag_sets;
        }

        const std::vector<json> tag_sets_json = (*json_obj)["Tag Sets"];

        // Loop through each json tag set object
        for (const json& tag_set_json : tag_sets_json) {
            std::optional<TagSet> tag_set = convertJsonTagSet(tag_set_json);

            // If tag_set is valid, add to tag_sets map
            if (tag_set) {
                tag_sets.insert(std::make_pair(tag_set.value().id, tag_set.value()));
            }
        }

        return tag_sets;
    }

}
