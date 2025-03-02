/*
 * Copyright (c) 2020-2022, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/CSS/Parser/Parser.h>
#include <LibWeb/HTML/HTMLTableCellElement.h>
#include <LibWeb/HTML/Parser/HTMLParser.h>

namespace Web::HTML {

HTMLTableCellElement::HTMLTableCellElement(DOM::Document& document, DOM::QualifiedName qualified_name)
    : HTMLElement(document, move(qualified_name))
{
    set_prototype(&Bindings::cached_web_prototype(realm(), "HTMLTableCellElement"));
}

HTMLTableCellElement::~HTMLTableCellElement() = default;

void HTMLTableCellElement::apply_presentational_hints(CSS::StyleProperties& style) const
{
    for_each_attribute([&](auto& name, auto& value) {
        if (name == HTML::AttributeNames::bgcolor) {
            auto color = Color::from_string(value);
            if (color.has_value())
                style.set_property(CSS::PropertyID::BackgroundColor, CSS::ColorStyleValue::create(color.value()));
            return;
        }
        if (name == HTML::AttributeNames::align) {
            if (value.equals_ignoring_case("center"sv) || value.equals_ignoring_case("middle"sv)) {
                style.set_property(CSS::PropertyID::TextAlign, CSS::IdentifierStyleValue::create(CSS::ValueID::LibwebCenter));
            } else {
                if (auto parsed_value = parse_css_value(CSS::Parser::ParsingContext { document() }, value.view(), CSS::PropertyID::TextAlign))
                    style.set_property(CSS::PropertyID::TextAlign, parsed_value.release_nonnull());
            }
            return;
        }
        if (name == HTML::AttributeNames::width) {
            if (auto parsed_value = parse_nonzero_dimension_value(value))
                style.set_property(CSS::PropertyID::Width, parsed_value.release_nonnull());
            return;
        } else if (name == HTML::AttributeNames::height) {
            if (auto parsed_value = parse_nonzero_dimension_value(value))
                style.set_property(CSS::PropertyID::Height, parsed_value.release_nonnull());
            return;
        }
    });
}

unsigned int HTMLTableCellElement::col_span() const
{
    return attribute(HTML::AttributeNames::colspan).to_uint().value_or(1);
}

void HTMLTableCellElement::set_col_span(unsigned int value)
{
    MUST(set_attribute(HTML::AttributeNames::colspan, DeprecatedString::number(value)));
}

unsigned int HTMLTableCellElement::row_span() const
{
    return attribute(HTML::AttributeNames::rowspan).to_uint().value_or(1);
}

void HTMLTableCellElement::set_row_span(unsigned int value)
{
    MUST(set_attribute(HTML::AttributeNames::rowspan, DeprecatedString::number(value)));
}

FlyString HTMLTableCellElement::default_role() const
{
    // TODO: For td:
    //       role=cell if the ancestor table element is exposed as a role=table
    //       role=gridcell if the ancestor table element is exposed as a role=grid or treegrid
    //       No corresponding role if the ancestor table element is not exposed as a role=table, grid or treegrid
    //       For th:
    //       role=columnheader, rowheader or cell if the ancestor table element is exposed as a role=table
    //        role=columnheader, rowheader or gridcell if the ancestor table element is exposed as a role=grid or treegrid
    //        No corresponding role if the ancestor table element is not exposed as a role=table, grid or treegrid
    // https://www.w3.org/TR/html-aria/#el-td
    // https://www.w3.org/TR/html-aria/#el-th
    return {};
}

}
