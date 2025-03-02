/*
 * Copyright (c) 2018-2022, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/AnyOf.h>
#include <AK/Debug.h>
#include <AK/StringBuilder.h>
#include <LibWeb/Bindings/ElementPrototype.h>
#include <LibWeb/CSS/Parser/Parser.h>
#include <LibWeb/CSS/PropertyID.h>
#include <LibWeb/CSS/ResolvedCSSStyleDeclaration.h>
#include <LibWeb/CSS/SelectorEngine.h>
#include <LibWeb/DOM/DOMTokenList.h>
#include <LibWeb/DOM/Document.h>
#include <LibWeb/DOM/Element.h>
#include <LibWeb/DOM/HTMLCollection.h>
#include <LibWeb/DOM/ShadowRoot.h>
#include <LibWeb/DOM/Text.h>
#include <LibWeb/DOMParsing/InnerHTML.h>
#include <LibWeb/Geometry/DOMRect.h>
#include <LibWeb/Geometry/DOMRectList.h>
#include <LibWeb/HTML/BrowsingContext.h>
#include <LibWeb/HTML/EventLoop/EventLoop.h>
#include <LibWeb/HTML/HTMLBodyElement.h>
#include <LibWeb/HTML/HTMLButtonElement.h>
#include <LibWeb/HTML/HTMLFieldSetElement.h>
#include <LibWeb/HTML/HTMLFrameSetElement.h>
#include <LibWeb/HTML/HTMLHtmlElement.h>
#include <LibWeb/HTML/HTMLInputElement.h>
#include <LibWeb/HTML/HTMLOptGroupElement.h>
#include <LibWeb/HTML/HTMLOptionElement.h>
#include <LibWeb/HTML/HTMLSelectElement.h>
#include <LibWeb/HTML/HTMLTextAreaElement.h>
#include <LibWeb/HTML/Parser/HTMLParser.h>
#include <LibWeb/Infra/CharacterTypes.h>
#include <LibWeb/Layout/BlockContainer.h>
#include <LibWeb/Layout/InitialContainingBlock.h>
#include <LibWeb/Layout/InlineNode.h>
#include <LibWeb/Layout/ListItemBox.h>
#include <LibWeb/Layout/TableBox.h>
#include <LibWeb/Layout/TableCellBox.h>
#include <LibWeb/Layout/TableRowBox.h>
#include <LibWeb/Layout/TableRowGroupBox.h>
#include <LibWeb/Layout/TreeBuilder.h>
#include <LibWeb/Namespace.h>
#include <LibWeb/Page/Page.h>
#include <LibWeb/Painting/PaintableBox.h>
#include <LibWeb/WebIDL/DOMException.h>
#include <LibWeb/WebIDL/ExceptionOr.h>

namespace Web::DOM {

Element::Element(Document& document, DOM::QualifiedName qualified_name)
    : ParentNode(document, NodeType::ELEMENT_NODE)
    , m_qualified_name(move(qualified_name))
{
    set_prototype(&Bindings::cached_web_prototype(document.realm(), "Element"));
    make_html_uppercased_qualified_name();
}

Element::~Element() = default;

void Element::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    m_attributes = NamedNodeMap::create(*this);
}

void Element::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_attributes.ptr());
    visitor.visit(m_inline_style.ptr());
    visitor.visit(m_class_list.ptr());
    visitor.visit(m_shadow_root.ptr());
    for (auto& pseudo_element_layout_node : m_pseudo_element_nodes)
        visitor.visit(pseudo_element_layout_node);
}

// https://dom.spec.whatwg.org/#dom-element-getattribute
DeprecatedString Element::get_attribute(FlyString const& name) const
{
    // 1. Let attr be the result of getting an attribute given qualifiedName and this.
    auto const* attribute = m_attributes->get_attribute(name);

    // 2. If attr is null, return null.
    if (!attribute)
        return {};

    // 3. Return attr’s value.
    return attribute->value();
}

// https://dom.spec.whatwg.org/#dom-element-getattributenode
JS::GCPtr<Attr> Element::get_attribute_node(FlyString const& name) const
{
    // The getAttributeNode(qualifiedName) method steps are to return the result of getting an attribute given qualifiedName and this.
    return m_attributes->get_attribute(name);
}

// https://dom.spec.whatwg.org/#dom-element-setattribute
WebIDL::ExceptionOr<void> Element::set_attribute(FlyString const& name, DeprecatedString const& value)
{
    // 1. If qualifiedName does not match the Name production in XML, then throw an "InvalidCharacterError" DOMException.
    // FIXME: Proper name validation
    if (name.is_empty())
        return WebIDL::InvalidCharacterError::create(realm(), "Attribute name must not be empty");

    // 2. If this is in the HTML namespace and its node document is an HTML document, then set qualifiedName to qualifiedName in ASCII lowercase.
    // FIXME: Handle the second condition, assume it is an HTML document for now.
    bool insert_as_lowercase = namespace_uri() == Namespace::HTML;

    // 3. Let attribute be the first attribute in this’s attribute list whose qualified name is qualifiedName, and null otherwise.
    auto* attribute = m_attributes->get_attribute(name);

    // 4. If attribute is null, create an attribute whose local name is qualifiedName, value is value, and node document is this’s node document, then append this attribute to this, and then return.
    if (!attribute) {
        auto new_attribute = Attr::create(document(), insert_as_lowercase ? name.to_lowercase() : name, value);
        m_attributes->append_attribute(new_attribute);

        attribute = new_attribute.ptr();
    }

    // 5. Change attribute to value.
    else {
        attribute->set_value(value);
    }

    parse_attribute(attribute->local_name(), value);

    invalidate_style_after_attribute_change(name);

    return {};
}

// https://dom.spec.whatwg.org/#validate-and-extract
WebIDL::ExceptionOr<QualifiedName> validate_and_extract(JS::Realm& realm, FlyString namespace_, FlyString qualified_name)
{
    // 1. If namespace is the empty string, then set it to null.
    if (namespace_.is_empty())
        namespace_ = {};

    // 2. Validate qualifiedName.
    TRY(Document::validate_qualified_name(realm, qualified_name));

    // 3. Let prefix be null.
    FlyString prefix = {};

    // 4. Let localName be qualifiedName.
    auto local_name = qualified_name;

    // 5. If qualifiedName contains a U+003A (:), then strictly split the string on it and set prefix to the part before and localName to the part after.
    if (qualified_name.view().contains(':')) {
        auto parts = qualified_name.view().split_view(':');
        prefix = parts[0];
        local_name = parts[1];
    }

    // 6. If prefix is non-null and namespace is null, then throw a "NamespaceError" DOMException.
    if (!prefix.is_null() && namespace_.is_null())
        return WebIDL::NamespaceError::create(realm, "Prefix is non-null and namespace is null.");

    // 7. If prefix is "xml" and namespace is not the XML namespace, then throw a "NamespaceError" DOMException.
    if (prefix == "xml"sv && namespace_ != Namespace::XML)
        return WebIDL::NamespaceError::create(realm, "Prefix is 'xml' and namespace is not the XML namespace.");

    // 8. If either qualifiedName or prefix is "xmlns" and namespace is not the XMLNS namespace, then throw a "NamespaceError" DOMException.
    if ((qualified_name == "xmlns"sv || prefix == "xmlns"sv) && namespace_ != Namespace::XMLNS)
        return WebIDL::NamespaceError::create(realm, "Either qualifiedName or prefix is 'xmlns' and namespace is not the XMLNS namespace.");

    // 9. If namespace is the XMLNS namespace and neither qualifiedName nor prefix is "xmlns", then throw a "NamespaceError" DOMException.
    if (namespace_ == Namespace::XMLNS && !(qualified_name == "xmlns"sv || prefix == "xmlns"sv))
        return WebIDL::NamespaceError::create(realm, "Namespace is the XMLNS namespace and neither qualifiedName nor prefix is 'xmlns'.");

    // 10. Return namespace, prefix, and localName.
    return QualifiedName { local_name, prefix, namespace_ };
}

// https://dom.spec.whatwg.org/#dom-element-setattributens
WebIDL::ExceptionOr<void> Element::set_attribute_ns(FlyString const& namespace_, FlyString const& qualified_name, DeprecatedString const& value)
{
    // 1. Let namespace, prefix, and localName be the result of passing namespace and qualifiedName to validate and extract.
    auto extracted_qualified_name = TRY(validate_and_extract(realm(), namespace_, qualified_name));

    // FIXME: 2. Set an attribute value for this using localName, value, and also prefix and namespace.

    // FIXME: Don't just call through to setAttribute() here.
    return set_attribute(extracted_qualified_name.local_name(), value);
}

// https://dom.spec.whatwg.org/#dom-element-removeattribute
void Element::remove_attribute(FlyString const& name)
{
    m_attributes->remove_attribute(name);

    did_remove_attribute(name);

    invalidate_style_after_attribute_change(name);
}

// https://dom.spec.whatwg.org/#dom-element-hasattribute
bool Element::has_attribute(FlyString const& name) const
{
    return m_attributes->get_attribute(name) != nullptr;
}

// https://dom.spec.whatwg.org/#dom-element-toggleattribute
WebIDL::ExceptionOr<bool> Element::toggle_attribute(FlyString const& name, Optional<bool> force)
{
    // 1. If qualifiedName does not match the Name production in XML, then throw an "InvalidCharacterError" DOMException.
    // FIXME: Proper name validation
    if (name.is_empty())
        return WebIDL::InvalidCharacterError::create(realm(), "Attribute name must not be empty");

    // 2. If this is in the HTML namespace and its node document is an HTML document, then set qualifiedName to qualifiedName in ASCII lowercase.
    // FIXME: Handle the second condition, assume it is an HTML document for now.
    bool insert_as_lowercase = namespace_uri() == Namespace::HTML;

    // 3. Let attribute be the first attribute in this’s attribute list whose qualified name is qualifiedName, and null otherwise.
    auto* attribute = m_attributes->get_attribute(name);

    // 4. If attribute is null, then:
    if (!attribute) {
        // 1. If force is not given or is true, create an attribute whose local name is qualifiedName, value is the empty string, and node document is this’s node document, then append this attribute to this, and then return true.
        if (!force.has_value() || force.value()) {
            auto new_attribute = Attr::create(document(), insert_as_lowercase ? name.to_lowercase() : name, "");
            m_attributes->append_attribute(new_attribute);

            parse_attribute(new_attribute->local_name(), "");

            invalidate_style_after_attribute_change(name);

            return true;
        }

        // 2. Return false.
        return false;
    }

    // 5. Otherwise, if force is not given or is false, remove an attribute given qualifiedName and this, and then return false.
    if (!force.has_value() || !force.value()) {
        m_attributes->remove_attribute(name);

        did_remove_attribute(name);

        invalidate_style_after_attribute_change(name);
    }

    // 6. Return true.
    return true;
}

// https://dom.spec.whatwg.org/#dom-element-getattributenames
Vector<DeprecatedString> Element::get_attribute_names() const
{
    // The getAttributeNames() method steps are to return the qualified names of the attributes in this’s attribute list, in order; otherwise a new list.
    Vector<DeprecatedString> names;
    for (size_t i = 0; i < m_attributes->length(); ++i) {
        auto const* attribute = m_attributes->item(i);
        names.append(attribute->name());
    }
    return names;
}

bool Element::has_class(FlyString const& class_name, CaseSensitivity case_sensitivity) const
{
    if (case_sensitivity == CaseSensitivity::CaseSensitive) {
        return any_of(m_classes, [&](auto& it) {
            return it == class_name;
        });
    } else {
        return any_of(m_classes, [&](auto& it) {
            return it.equals_ignoring_case(class_name);
        });
    }
}

JS::GCPtr<Layout::Node> Element::create_layout_node(NonnullRefPtr<CSS::StyleProperties> style)
{
    if (local_name() == "noscript" && document().is_scripting_enabled())
        return nullptr;

    auto display = style->display();
    return create_layout_node_for_display_type(document(), display, move(style), this);
}

JS::GCPtr<Layout::Node> Element::create_layout_node_for_display_type(DOM::Document& document, CSS::Display const& display, NonnullRefPtr<CSS::StyleProperties> style, Element* element)
{
    if (display.is_table_inside())
        return document.heap().allocate_without_realm<Layout::TableBox>(document, element, move(style));

    if (display.is_list_item())
        return document.heap().allocate_without_realm<Layout::ListItemBox>(document, element, move(style));

    if (display.is_table_row())
        return document.heap().allocate_without_realm<Layout::TableRowBox>(document, element, move(style));

    if (display.is_table_cell())
        return document.heap().allocate_without_realm<Layout::TableCellBox>(document, element, move(style));

    if (display.is_table_row_group() || display.is_table_header_group() || display.is_table_footer_group())
        return document.heap().allocate_without_realm<Layout::TableRowGroupBox>(document, element, move(style));

    if (display.is_table_column() || display.is_table_column_group() || display.is_table_caption()) {
        // FIXME: This is just an incorrect placeholder until we improve table layout support.
        return document.heap().allocate_without_realm<Layout::BlockContainer>(document, element, move(style));
    }

    if (display.is_inline_outside()) {
        if (display.is_flow_root_inside())
            return document.heap().allocate_without_realm<Layout::BlockContainer>(document, element, move(style));
        if (display.is_flow_inside())
            return document.heap().allocate_without_realm<Layout::InlineNode>(document, element, move(style));
        if (display.is_flex_inside())
            return document.heap().allocate_without_realm<Layout::BlockContainer>(document, element, move(style));
        dbgln_if(LIBWEB_CSS_DEBUG, "FIXME: Support display: {}", display.to_deprecated_string());
        return document.heap().allocate_without_realm<Layout::InlineNode>(document, element, move(style));
    }

    if (display.is_flow_inside() || display.is_flow_root_inside() || display.is_flex_inside() || display.is_grid_inside())
        return document.heap().allocate_without_realm<Layout::BlockContainer>(document, element, move(style));

    TODO();
}

CSS::CSSStyleDeclaration const* Element::inline_style() const
{
    return m_inline_style.ptr();
}

void Element::parse_attribute(FlyString const& name, DeprecatedString const& value)
{
    if (name == HTML::AttributeNames::class_) {
        auto new_classes = value.split_view(Infra::is_ascii_whitespace);
        m_classes.clear();
        m_classes.ensure_capacity(new_classes.size());
        for (auto& new_class : new_classes) {
            m_classes.unchecked_append(new_class);
        }
        if (m_class_list)
            m_class_list->associated_attribute_changed(value);
    } else if (name == HTML::AttributeNames::style) {
        // https://drafts.csswg.org/cssom/#ref-for-cssstyledeclaration-updating-flag
        if (m_inline_style && m_inline_style->is_updating())
            return;
        m_inline_style = parse_css_style_attribute(CSS::Parser::ParsingContext(document()), value, *this);
        set_needs_style_update(true);
    }
}

void Element::did_remove_attribute(FlyString const& name)
{
    if (name == HTML::AttributeNames::style) {
        if (m_inline_style) {
            m_inline_style = nullptr;
            set_needs_style_update(true);
        }
    }
}

enum class RequiredInvalidation {
    None,
    RepaintOnly,
    RebuildStackingContextTree,
    Relayout,
};

static RequiredInvalidation compute_required_invalidation(CSS::StyleProperties const& old_style, CSS::StyleProperties const& new_style)
{
    if (&old_style.computed_font() != &new_style.computed_font())
        return RequiredInvalidation::Relayout;
    bool requires_repaint = false;
    bool requires_stacking_context_tree_rebuild = false;
    for (auto i = to_underlying(CSS::first_property_id); i <= to_underlying(CSS::last_property_id); ++i) {
        auto property_id = static_cast<CSS::PropertyID>(i);
        auto const& old_value = old_style.properties()[i];
        auto const& new_value = new_style.properties()[i];
        if (!old_value && !new_value)
            continue;
        if (!old_value || !new_value)
            return RequiredInvalidation::Relayout;
        if (*old_value == *new_value)
            continue;
        if (CSS::property_affects_layout(property_id))
            return RequiredInvalidation::Relayout;
        if (CSS::property_affects_stacking_context(property_id))
            requires_stacking_context_tree_rebuild = true;
        requires_repaint = true;
    }
    if (requires_stacking_context_tree_rebuild)
        return RequiredInvalidation::RebuildStackingContextTree;
    if (requires_repaint)
        return RequiredInvalidation::RepaintOnly;
    return RequiredInvalidation::None;
}

Element::NeedsRelayout Element::recompute_style()
{
    set_needs_style_update(false);
    VERIFY(parent());

    // FIXME propagate errors
    auto new_computed_css_values = MUST(document().style_computer().compute_style(*this));

    auto required_invalidation = RequiredInvalidation::Relayout;

    if (m_computed_css_values)
        required_invalidation = compute_required_invalidation(*m_computed_css_values, *new_computed_css_values);

    if (required_invalidation == RequiredInvalidation::None)
        return NeedsRelayout::No;

    m_computed_css_values = move(new_computed_css_values);

    if (required_invalidation == RequiredInvalidation::RepaintOnly && layout_node()) {
        layout_node()->apply_style(*m_computed_css_values);
        layout_node()->set_needs_display();
        return NeedsRelayout::No;
    }

    if (required_invalidation == RequiredInvalidation::RebuildStackingContextTree && layout_node()) {
        layout_node()->apply_style(*m_computed_css_values);
        document().invalidate_stacking_context_tree();
        layout_node()->set_needs_display();
        return NeedsRelayout::No;
    }

    return NeedsRelayout::Yes;
}

NonnullRefPtr<CSS::StyleProperties> Element::resolved_css_values()
{
    auto element_computed_style = CSS::ResolvedCSSStyleDeclaration::create(*this);
    auto properties = CSS::StyleProperties::create();

    for (auto i = to_underlying(CSS::first_property_id); i <= to_underlying(CSS::last_property_id); ++i) {
        auto property_id = (CSS::PropertyID)i;
        auto maybe_value = element_computed_style->property(property_id);
        if (!maybe_value.has_value())
            continue;
        properties->set_property(property_id, maybe_value.release_value().value);
    }

    return properties;
}

DOMTokenList* Element::class_list()
{
    if (!m_class_list)
        m_class_list = DOMTokenList::create(*this, HTML::AttributeNames::class_);
    return m_class_list;
}

// https://dom.spec.whatwg.org/#dom-element-matches
WebIDL::ExceptionOr<bool> Element::matches(StringView selectors) const
{
    auto maybe_selectors = parse_selector(CSS::Parser::ParsingContext(static_cast<ParentNode&>(const_cast<Element&>(*this))), selectors);
    if (!maybe_selectors.has_value())
        return WebIDL::SyntaxError::create(realm(), "Failed to parse selector");

    auto sel = maybe_selectors.value();
    for (auto& s : sel) {
        if (SelectorEngine::matches(s, *this))
            return true;
    }
    return false;
}

// https://dom.spec.whatwg.org/#dom-element-closest
WebIDL::ExceptionOr<DOM::Element const*> Element::closest(StringView selectors) const
{
    auto maybe_selectors = parse_selector(CSS::Parser::ParsingContext(static_cast<ParentNode&>(const_cast<Element&>(*this))), selectors);
    if (!maybe_selectors.has_value())
        return WebIDL::SyntaxError::create(realm(), "Failed to parse selector");

    auto matches_selectors = [](CSS::SelectorList const& selector_list, Element const* element) {
        for (auto& selector : selector_list) {
            if (!SelectorEngine::matches(selector, *element))
                return false;
        }
        return true;
    };

    auto const selector_list = maybe_selectors.release_value();
    for (auto* element = this; element; element = element->parent_element()) {
        if (!matches_selectors(selector_list, element))
            continue;

        return element;
    }

    return nullptr;
}

WebIDL::ExceptionOr<void> Element::set_inner_html(DeprecatedString const& markup)
{
    TRY(DOMParsing::inner_html_setter(*this, markup));
    return {};
}

// https://w3c.github.io/DOM-Parsing/#dom-innerhtml-innerhtml
WebIDL::ExceptionOr<DeprecatedString> Element::inner_html() const
{
    return serialize_fragment(DOMParsing::RequireWellFormed::Yes);
}

bool Element::is_focused() const
{
    return document().focused_element() == this;
}

bool Element::is_active() const
{
    return document().active_element() == this;
}

JS::NonnullGCPtr<HTMLCollection> Element::get_elements_by_class_name(FlyString const& class_names)
{
    Vector<FlyString> list_of_class_names;
    for (auto& name : class_names.view().split_view_if(Infra::is_ascii_whitespace)) {
        list_of_class_names.append(name);
    }
    return HTMLCollection::create(*this, [list_of_class_names = move(list_of_class_names), quirks_mode = document().in_quirks_mode()](Element const& element) {
        for (auto& name : list_of_class_names) {
            if (!element.has_class(name, quirks_mode ? CaseSensitivity::CaseInsensitive : CaseSensitivity::CaseSensitive))
                return false;
        }
        return true;
    });
}

void Element::set_shadow_root(JS::GCPtr<ShadowRoot> shadow_root)
{
    if (m_shadow_root == shadow_root)
        return;
    if (m_shadow_root)
        m_shadow_root->set_host(nullptr);
    m_shadow_root = move(shadow_root);
    if (m_shadow_root)
        m_shadow_root->set_host(this);
    invalidate_style();
}

CSS::CSSStyleDeclaration* Element::style_for_bindings()
{
    if (!m_inline_style)
        m_inline_style = CSS::ElementInlineCSSStyleDeclaration::create(*this, {}, {});
    return m_inline_style;
}

// https://dom.spec.whatwg.org/#element-html-uppercased-qualified-name
void Element::make_html_uppercased_qualified_name()
{
    // This is allowed by the spec: "User agents could optimize qualified name and HTML-uppercased qualified name by storing them in internal slots."
    if (namespace_() == Namespace::HTML && document().document_type() == Document::Type::HTML)
        m_html_uppercased_qualified_name = qualified_name().to_uppercase();
    else
        m_html_uppercased_qualified_name = qualified_name();
}

// https://html.spec.whatwg.org/multipage/webappapis.html#queue-an-element-task
void Element::queue_an_element_task(HTML::Task::Source source, JS::SafeFunction<void()> steps)
{
    auto task = HTML::Task::create(source, &document(), move(steps));
    HTML::main_thread_event_loop().task_queue().add(move(task));
}

// https://html.spec.whatwg.org/multipage/syntax.html#void-elements
bool Element::is_void_element() const
{
    return local_name().is_one_of(HTML::TagNames::area, HTML::TagNames::base, HTML::TagNames::br, HTML::TagNames::col, HTML::TagNames::embed, HTML::TagNames::hr, HTML::TagNames::img, HTML::TagNames::input, HTML::TagNames::link, HTML::TagNames::meta, HTML::TagNames::param, HTML::TagNames::source, HTML::TagNames::track, HTML::TagNames::wbr);
}

// https://html.spec.whatwg.org/multipage/parsing.html#serializes-as-void
bool Element::serializes_as_void() const
{
    return is_void_element() || local_name().is_one_of(HTML::TagNames::basefont, HTML::TagNames::bgsound, HTML::TagNames::frame, HTML::TagNames::keygen);
}

// https://drafts.csswg.org/cssom-view/#dom-element-getboundingclientrect
JS::NonnullGCPtr<Geometry::DOMRect> Element::get_bounding_client_rect() const
{
    // // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // FIXME: Support inline layout nodes as well.
    auto* paint_box = this->paint_box();
    if (!paint_box)
        return Geometry::DOMRect::construct_impl(realm(), 0, 0, 0, 0);

    VERIFY(document().browsing_context());
    auto viewport_offset = document().browsing_context()->viewport_scroll_offset();

    return Geometry::DOMRect::create(realm(), paint_box->absolute_rect().translated(-viewport_offset.x(), -viewport_offset.y()).to_type<float>());
}

// https://drafts.csswg.org/cssom-view/#dom-element-getclientrects
JS::NonnullGCPtr<Geometry::DOMRectList> Element::get_client_rects() const
{
    Vector<JS::Handle<Geometry::DOMRect>> rects;

    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // 1. If the element on which it was invoked does not have an associated layout box return an empty DOMRectList object and stop this algorithm.
    if (!layout_node() || !layout_node()->is_box())
        return Geometry::DOMRectList::create(realm(), move(rects));

    // FIXME: 2. If the element has an associated SVG layout box return a DOMRectList object containing a single DOMRect object that describes
    // the bounding box of the element as defined by the SVG specification, applying the transforms that apply to the element and its ancestors.

    // FIXME: 3. Return a DOMRectList object containing DOMRect objects in content order, one for each box fragment,
    // describing its border area (including those with a height or width of zero) with the following constraints:
    // - Apply the transforms that apply to the element and its ancestors.
    // - If the element on which the method was invoked has a computed value for the display property of table
    // or inline-table include both the table box and the caption box, if any, but not the anonymous container box.
    // - Replace each anonymous block box with its child box(es) and repeat this until no anonymous block boxes are left in the final list.

    auto bounding_rect = get_bounding_client_rect();
    rects.append(*bounding_rect);
    return Geometry::DOMRectList::create(realm(), move(rects));
}

int Element::client_top() const
{
    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // 1. If the element has no associated CSS layout box or if the CSS layout box is inline, return zero.
    if (!layout_node() || !layout_node()->is_box())
        return 0;

    // 2. Return the computed value of the border-top-width property
    //    plus the height of any scrollbar rendered between the top padding edge and the top border edge,
    //    ignoring any transforms that apply to the element and its ancestors.
    return static_cast<Layout::Box const&>(*layout_node()).computed_values().border_top().width;
}

// https://drafts.csswg.org/cssom-view/#dom-element-clientleft
int Element::client_left() const
{
    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // 1. If the element has no associated CSS layout box or if the CSS layout box is inline, return zero.
    if (!layout_node() || !layout_node()->is_box())
        return 0;

    // 2. Return the computed value of the border-left-width property
    //    plus the width of any scrollbar rendered between the left padding edge and the left border edge,
    //    ignoring any transforms that apply to the element and its ancestors.
    return static_cast<Layout::Box const&>(*layout_node()).computed_values().border_left().width;
}

// https://drafts.csswg.org/cssom-view/#dom-element-clientwidth
int Element::client_width() const
{
    // NOTE: We do step 2 before step 1 here since step 2 can exit early without needing to perform layout.

    // 2. If the element is the root element and the element’s node document is not in quirks mode,
    //    or if the element is the HTML body element and the element’s node document is in quirks mode,
    //    return the viewport width excluding the size of a rendered scroll bar (if any).
    if ((is<HTML::HTMLHtmlElement>(*this) && !document().in_quirks_mode())
        || (is<HTML::HTMLBodyElement>(*this) && document().in_quirks_mode())) {
        return document().browsing_context()->viewport_rect().width().value();
    }

    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // 1. If the element has no associated CSS layout box or if the CSS layout box is inline, return zero.
    if (!paint_box())
        return 0;

    // 3. Return the width of the padding edge excluding the width of any rendered scrollbar between the padding edge and the border edge,
    // ignoring any transforms that apply to the element and its ancestors.
    return paint_box()->absolute_padding_box_rect().width().value();
}

// https://drafts.csswg.org/cssom-view/#dom-element-clientheight
int Element::client_height() const
{
    // NOTE: We do step 2 before step 1 here since step 2 can exit early without needing to perform layout.

    // 2. If the element is the root element and the element’s node document is not in quirks mode,
    //    or if the element is the HTML body element and the element’s node document is in quirks mode,
    //    return the viewport height excluding the size of a rendered scroll bar (if any).
    if ((is<HTML::HTMLHtmlElement>(*this) && !document().in_quirks_mode())
        || (is<HTML::HTMLBodyElement>(*this) && document().in_quirks_mode())) {
        return document().browsing_context()->viewport_rect().height().value();
    }

    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // 1. If the element has no associated CSS layout box or if the CSS layout box is inline, return zero.
    if (!paint_box())
        return 0;

    // 3. Return the height of the padding edge excluding the height of any rendered scrollbar between the padding edge and the border edge,
    //    ignoring any transforms that apply to the element and its ancestors.
    return paint_box()->absolute_padding_box_rect().height().value();
}

void Element::children_changed()
{
    Node::children_changed();
    set_needs_style_update(true);
}

void Element::set_pseudo_element_node(Badge<Layout::TreeBuilder>, CSS::Selector::PseudoElement pseudo_element, JS::GCPtr<Layout::Node> pseudo_element_node)
{
    m_pseudo_element_nodes[to_underlying(pseudo_element)] = pseudo_element_node;
}

JS::GCPtr<Layout::Node> Element::get_pseudo_element_node(CSS::Selector::PseudoElement pseudo_element) const
{
    return m_pseudo_element_nodes[to_underlying(pseudo_element)];
}

void Element::clear_pseudo_element_nodes(Badge<Layout::TreeBuilder>)
{
    m_pseudo_element_nodes.fill({});
}

void Element::serialize_pseudo_elements_as_json(JsonArraySerializer<StringBuilder>& children_array) const
{
    for (size_t i = 0; i < m_pseudo_element_nodes.size(); ++i) {
        auto& pseudo_element_node = m_pseudo_element_nodes[i];
        if (!pseudo_element_node)
            continue;
        auto object = MUST(children_array.add_object());
        MUST(object.add("name"sv, DeprecatedString::formatted("::{}", CSS::pseudo_element_name(static_cast<CSS::Selector::PseudoElement>(i)))));
        MUST(object.add("type"sv, "pseudo-element"));
        MUST(object.add("parent-id"sv, id()));
        MUST(object.add("pseudo-element"sv, i));
        MUST(object.finish());
    }
}

// https://html.spec.whatwg.org/multipage/interaction.html#dom-tabindex
i32 Element::default_tab_index_value() const
{
    // The default value is 0 if the element is an a, area, button, frame, iframe, input, object, select, textarea, or SVG a element, or is a summary element that is a summary for its parent details.
    // The default value is −1 otherwise.
    // Note: The varying default value based on element type is a historical artifact.
    // FIXME: We currently do not have the SVG a element.
    return -1;
}

// https://html.spec.whatwg.org/multipage/interaction.html#dom-tabindex
i32 Element::tab_index() const
{
    // FIXME: I'm not sure if "to_int" exactly matches the specs "rules for parsing integers"
    auto maybe_table_index = attribute(HTML::AttributeNames::tabindex).to_int<i32>();
    if (!maybe_table_index.has_value())
        return default_tab_index_value();
    return maybe_table_index.value();
}

// https://html.spec.whatwg.org/multipage/interaction.html#dom-tabindex
void Element::set_tab_index(i32 tab_index)
{
    MUST(set_attribute(HTML::AttributeNames::tabindex, DeprecatedString::number(tab_index)));
}

// https://drafts.csswg.org/cssom-view/#potentially-scrollable
bool Element::is_potentially_scrollable() const
{
    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document()).update_layout();

    // An element body (which will be the body element) is potentially scrollable if all of the following conditions are true:
    VERIFY(is<HTML::HTMLBodyElement>(this) || is<HTML::HTMLFrameSetElement>(this));

    // Since this should always be the body element, the body element must have a <html> element parent. See Document::body().
    VERIFY(parent());

    // - body has an associated box.
    // - body’s parent element’s computed value of the overflow-x or overflow-y properties is neither visible nor clip.
    // - body’s computed value of the overflow-x or overflow-y properties is neither visible nor clip.
    return layout_node()
        && (parent()->layout_node()
            && parent()->layout_node()->computed_values().overflow_x() != CSS::Overflow::Visible && parent()->layout_node()->computed_values().overflow_x() != CSS::Overflow::Clip
            && parent()->layout_node()->computed_values().overflow_y() != CSS::Overflow::Visible && parent()->layout_node()->computed_values().overflow_y() != CSS::Overflow::Clip)
        && (layout_node()->computed_values().overflow_x() != CSS::Overflow::Visible && layout_node()->computed_values().overflow_x() != CSS::Overflow::Clip
            && layout_node()->computed_values().overflow_y() != CSS::Overflow::Visible && layout_node()->computed_values().overflow_y() != CSS::Overflow::Clip);
}

// https://drafts.csswg.org/cssom-view/#dom-element-scrolltop
double Element::scroll_top() const
{
    // 1. Let document be the element’s node document.
    auto& document = this->document();

    // 2. If document is not the active document, return zero and terminate these steps.
    if (!document.is_active())
        return 0.0;

    // 3. Let window be the value of document’s defaultView attribute.
    auto* window = document.default_view();

    // 4. If window is null, return zero and terminate these steps.
    if (!window)
        return 0.0;

    // 5. If the element is the root element and document is in quirks mode, return zero and terminate these steps.
    if (document.document_element() == this && document.in_quirks_mode())
        return 0.0;

    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document).update_layout();

    // 6. If the element is the root element return the value of scrollY on window.
    if (document.document_element() == this)
        return window->scroll_y();

    // 7. If the element is the body element, document is in quirks mode, and the element is not potentially scrollable, return the value of scrollY on window.
    if (document.body() == this && document.in_quirks_mode() && !is_potentially_scrollable())
        return window->scroll_y();

    // 8. If the element does not have any associated box, return zero and terminate these steps.
    if (!layout_node() || !is<Layout::BlockContainer>(layout_node()))
        return 0.0;

    auto const* block_container = static_cast<Layout::BlockContainer const*>(layout_node());

    // 9. Return the y-coordinate of the scrolling area at the alignment point with the top of the padding edge of the element.
    // FIXME: Is this correct?
    return block_container->scroll_offset().y().value();
}

double Element::scroll_left() const
{
    // 1. Let document be the element’s node document.
    auto& document = this->document();

    // 2. If document is not the active document, return zero and terminate these steps.
    if (!document.is_active())
        return 0.0;

    // 3. Let window be the value of document’s defaultView attribute.
    auto* window = document.default_view();

    // 4. If window is null, return zero and terminate these steps.
    if (!window)
        return 0.0;

    // 5. If the element is the root element and document is in quirks mode, return zero and terminate these steps.
    if (document.document_element() == this && document.in_quirks_mode())
        return 0.0;

    // NOTE: Ensure that layout is up-to-date before looking at metrics.
    const_cast<Document&>(document).update_layout();

    // 6. If the element is the root element return the value of scrollX on window.
    if (document.document_element() == this)
        return window->scroll_x();

    // 7. If the element is the body element, document is in quirks mode, and the element is not potentially scrollable, return the value of scrollX on window.
    if (document.body() == this && document.in_quirks_mode() && !is_potentially_scrollable())
        return window->scroll_x();

    // 8. If the element does not have any associated box, return zero and terminate these steps.
    if (!layout_node() || !is<Layout::BlockContainer>(layout_node()))
        return 0.0;

    auto const* block_container = static_cast<Layout::BlockContainer const*>(layout_node());

    // 9. Return the x-coordinate of the scrolling area at the alignment point with the left of the padding edge of the element.
    // FIXME: Is this correct?
    return block_container->scroll_offset().x().value();
}

// https://drafts.csswg.org/cssom-view/#dom-element-scrollleft
void Element::set_scroll_left(double x)
{
    // 1. Let x be the given value.

    // 2. Normalize non-finite values for x.
    if (!isfinite(x))
        x = 0.0;

    // 3. Let document be the element’s node document.
    auto& document = this->document();

    // 4. If document is not the active document, terminate these steps.
    if (!document.is_active())
        return;

    // 5. Let window be the value of document’s defaultView attribute.
    auto* window = document.default_view();

    // 6. If window is null, terminate these steps.
    if (!window)
        return;

    // 7. If the element is the root element and document is in quirks mode, terminate these steps.
    if (document.document_element() == this && document.in_quirks_mode())
        return;

    // NOTE: Ensure that layout is up-to-date before looking at metrics or scrolling the page.
    const_cast<Document&>(document).update_layout();

    // 8. If the element is the root element invoke scroll() on window with x as first argument and scrollY on window as second argument, and terminate these steps.
    if (document.document_element() == this) {
        // FIXME: Implement this in terms of invoking scroll() on window.
        if (auto* page = document.page())
            page->client().page_did_request_scroll_to({ static_cast<float>(x), window->scroll_y() });

        return;
    }

    // 9. If the element is the body element, document is in quirks mode, and the element is not potentially scrollable, invoke scroll() on window with x as first argument and scrollY on window as second argument, and terminate these steps.
    if (document.body() == this && document.in_quirks_mode() && !is_potentially_scrollable()) {
        // FIXME: Implement this in terms of invoking scroll() on window.
        if (auto* page = document.page())
            page->client().page_did_request_scroll_to({ static_cast<float>(x), window->scroll_y() });

        return;
    }

    // 10. If the element does not have any associated box, the element has no associated scrolling box, or the element has no overflow, terminate these steps.
    if (!layout_node() || !is<Layout::BlockContainer>(layout_node()))
        return;

    auto* block_container = static_cast<Layout::BlockContainer*>(layout_node());
    if (!block_container->is_scrollable())
        return;

    // FIXME: or the element has no overflow.

    // 11. Scroll the element to x,scrollTop, with the scroll behavior being "auto".
    // FIXME: Implement this in terms of calling "scroll the element".
    auto scroll_offset = block_container->scroll_offset();
    scroll_offset.set_x(static_cast<float>(x));
    block_container->set_scroll_offset(scroll_offset);
}

void Element::set_scroll_top(double y)
{
    // 1. Let y be the given value.

    // 2. Normalize non-finite values for y.
    if (!isfinite(y))
        y = 0.0;

    // 3. Let document be the element’s node document.
    auto& document = this->document();

    // 4. If document is not the active document, terminate these steps.
    if (!document.is_active())
        return;

    // 5. Let window be the value of document’s defaultView attribute.
    auto* window = document.default_view();

    // 6. If window is null, terminate these steps.
    if (!window)
        return;

    // 7. If the element is the root element and document is in quirks mode, terminate these steps.
    if (document.document_element() == this && document.in_quirks_mode())
        return;

    // NOTE: Ensure that layout is up-to-date before looking at metrics or scrolling the page.
    const_cast<Document&>(document).update_layout();

    // 8. If the element is the root element invoke scroll() on window with scrollX on window as first argument and y as second argument, and terminate these steps.
    if (document.document_element() == this) {
        // FIXME: Implement this in terms of invoking scroll() on window.
        if (auto* page = document.page())
            page->client().page_did_request_scroll_to({ window->scroll_x(), static_cast<float>(y) });

        return;
    }

    // 9. If the element is the body element, document is in quirks mode, and the element is not potentially scrollable, invoke scroll() on window with scrollX as first argument and y as second argument, and terminate these steps.
    if (document.body() == this && document.in_quirks_mode() && !is_potentially_scrollable()) {
        // FIXME: Implement this in terms of invoking scroll() on window.
        if (auto* page = document.page())
            page->client().page_did_request_scroll_to({ window->scroll_x(), static_cast<float>(y) });

        return;
    }

    // 10. If the element does not have any associated box, the element has no associated scrolling box, or the element has no overflow, terminate these steps.
    if (!layout_node() || !is<Layout::BlockContainer>(layout_node()))
        return;

    auto* block_container = static_cast<Layout::BlockContainer*>(layout_node());
    if (!block_container->is_scrollable())
        return;

    // FIXME: or the element has no overflow.

    // 11. Scroll the element to scrollLeft,y, with the scroll behavior being "auto".
    // FIXME: Implement this in terms of calling "scroll the element".
    auto scroll_offset = block_container->scroll_offset();
    scroll_offset.set_y(static_cast<float>(y));
    block_container->set_scroll_offset(scroll_offset);
}

int Element::scroll_width() const
{
    dbgln("FIXME: Implement Element::scroll_width() (called on element: {})", debug_description());
    return 0;
}

int Element::scroll_height() const
{
    dbgln("FIXME: Implement Element::scroll_height() (called on element: {})", debug_description());
    return 0;
}

// https://html.spec.whatwg.org/multipage/semantics-other.html#concept-element-disabled
bool Element::is_actually_disabled() const
{
    // An element is said to be actually disabled if it is one of the following:
    // - a button element that is disabled
    // - an input element that is disabled
    // - a select element that is disabled
    // - a textarea element that is disabled
    if (is<HTML::HTMLButtonElement>(this) || is<HTML::HTMLInputElement>(this) || is<HTML::HTMLSelectElement>(this) || is<HTML::HTMLTextAreaElement>(this)) {
        auto const* form_associated_element = dynamic_cast<HTML::FormAssociatedElement const*>(this);
        VERIFY(form_associated_element);

        return !form_associated_element->enabled();
    }

    // - an optgroup element that has a disabled attribute
    if (is<HTML::HTMLOptGroupElement>(this))
        return has_attribute(HTML::AttributeNames::disabled);

    // - an option element that is disabled
    if (is<HTML::HTMLOptionElement>(this))
        return static_cast<HTML::HTMLOptionElement const&>(*this).disabled();

    // - a fieldset element that is a disabled fieldset
    if (is<HTML::HTMLFieldSetElement>(this))
        return static_cast<HTML::HTMLFieldSetElement const&>(*this).is_disabled();

    // FIXME: - a form-associated custom element that is disabled
    return false;
}

// https://w3c.github.io/DOM-Parsing/#dom-element-insertadjacenthtml
WebIDL::ExceptionOr<void> Element::insert_adjacent_html(DeprecatedString position, DeprecatedString text)
{
    JS::GCPtr<Node> context;
    // 1. Use the first matching item from this list:
    // - If position is an ASCII case-insensitive match for the string "beforebegin"
    // - If position is an ASCII case-insensitive match for the string "afterend"
    if (position.equals_ignoring_case("beforebegin"sv) || position.equals_ignoring_case("afterend"sv)) {
        // Let context be the context object's parent.
        context = this->parent();

        // If context is null or a Document, throw a "NoModificationAllowedError" DOMException.
        if (!context || context->is_document())
            return WebIDL::NoModificationAllowedError::create(realm(), "insertAdjacentHTML: context is null or a Document"sv);
    }
    // - If position is an ASCII case-insensitive match for the string "afterbegin"
    // - If position is an ASCII case-insensitive match for the string "beforeend"
    else if (position.equals_ignoring_case("afterbegin"sv) || position.equals_ignoring_case("beforeend"sv)) {
        // Let context be the context object.
        context = this;
    }
    // Otherwise
    else {
        // Throw a "SyntaxError" DOMException.
        return WebIDL::SyntaxError::create(realm(), "insertAdjacentHTML: invalid position argument"sv);
    }

    // 2. If context is not an Element or the following are all true:
    //    - context's node document is an HTML document,
    //    - context's local name is "html", and
    //    - context's namespace is the HTML namespace;
    if (!is<Element>(*context)
        || (context->document().document_type() == Document::Type::HTML
            && static_cast<Element const&>(*context).local_name() == "html"sv
            && static_cast<Element const&>(*context).namespace_() == Namespace::HTML)) {
        // FIXME: let context be a new Element with
        //        - body as its local name,
        //        - The HTML namespace as its namespace, and
        //        - The context object's node document as its node document.
        TODO();
    }

    // 3. Let fragment be the result of invoking the fragment parsing algorithm with text as markup, and context as the context element.
    auto fragment = TRY(DOMParsing::parse_fragment(text, verify_cast<Element>(*context)));

    // 4. Use the first matching item from this list:

    // - If position is an ASCII case-insensitive match for the string "beforebegin"
    if (position.equals_ignoring_case("beforebegin"sv)) {
        // Insert fragment into the context object's parent before the context object.
        parent()->insert_before(fragment, this);
    }

    // - If position is an ASCII case-insensitive match for the string "afterbegin"
    else if (position.equals_ignoring_case("afterbegin"sv)) {
        // Insert fragment into the context object before its first child.
        insert_before(fragment, first_child());
    }

    // - If position is an ASCII case-insensitive match for the string "beforeend"
    else if (position.equals_ignoring_case("beforeend"sv)) {
        // Append fragment to the context object.
        TRY(append_child(fragment));
    }

    // - If position is an ASCII case-insensitive match for the string "afterend"
    else if (position.equals_ignoring_case("afterend"sv)) {
        // Insert fragment into the context object's parent before the context object's next sibling.
        parent()->insert_before(fragment, next_sibling());
    }
    return {};
}

// https://dom.spec.whatwg.org/#insert-adjacent
WebIDL::ExceptionOr<JS::GCPtr<Node>> Element::insert_adjacent(DeprecatedString const& where, JS::NonnullGCPtr<Node> node)
{
    // To insert adjacent, given an element element, string where, and a node node, run the steps associated with the first ASCII case-insensitive match for where:
    if (where.equals_ignoring_case("beforebegin"sv)) {
        // -> "beforebegin"
        // If element’s parent is null, return null.
        if (!parent())
            return JS::GCPtr<Node> { nullptr };

        // Return the result of pre-inserting node into element’s parent before element.
        return JS::GCPtr<Node> { TRY(parent()->pre_insert(move(node), this)) };
    }

    if (where.equals_ignoring_case("afterbegin"sv)) {
        // -> "afterbegin"
        // Return the result of pre-inserting node into element before element’s first child.
        return JS::GCPtr<Node> { TRY(pre_insert(move(node), first_child())) };
    }

    if (where.equals_ignoring_case("beforeend"sv)) {
        // -> "beforeend"
        // Return the result of pre-inserting node into element before null.
        return JS::GCPtr<Node> { TRY(pre_insert(move(node), nullptr)) };
    }

    if (where.equals_ignoring_case("afterend"sv)) {
        // -> "afterend"
        // If element’s parent is null, return null.
        if (!parent())
            return JS::GCPtr<Node> { nullptr };

        // Return the result of pre-inserting node into element’s parent before element’s next sibling.
        return JS::GCPtr<Node> { TRY(parent()->pre_insert(move(node), next_sibling())) };
    }

    // -> Otherwise
    // Throw a "SyntaxError" DOMException.
    return WebIDL::SyntaxError::create(realm(), DeprecatedString::formatted("Unknown position '{}'. Must be one of 'beforebegin', 'afterbegin', 'beforeend' or 'afterend'"sv, where));
}

// https://dom.spec.whatwg.org/#dom-element-insertadjacentelement
WebIDL::ExceptionOr<JS::GCPtr<Element>> Element::insert_adjacent_element(DeprecatedString const& where, JS::NonnullGCPtr<Element> element)
{
    // The insertAdjacentElement(where, element) method steps are to return the result of running insert adjacent, give this, where, and element.
    auto returned_node = TRY(insert_adjacent(where, move(element)));
    if (!returned_node)
        return JS::GCPtr<Element> { nullptr };
    return JS::GCPtr<Element> { verify_cast<Element>(*returned_node) };
}

// https://dom.spec.whatwg.org/#dom-element-insertadjacenttext
WebIDL::ExceptionOr<void> Element::insert_adjacent_text(DeprecatedString const& where, DeprecatedString const& data)
{
    // 1. Let text be a new Text node whose data is data and node document is this’s node document.
    auto text = heap().allocate<DOM::Text>(realm(), document(), data);

    // 2. Run insert adjacent, given this, where, and text.
    // Spec Note: This method returns nothing because it existed before we had a chance to design it.
    (void)TRY(insert_adjacent(where, text));
    return {};
}

// https://w3c.github.io/csswg-drafts/cssom-view-1/#scroll-an-element-into-view
static ErrorOr<void> scroll_an_element_into_view(DOM::Element& element, Bindings::ScrollBehavior behavior, Bindings::ScrollLogicalPosition block, Bindings::ScrollLogicalPosition inline_)
{
    // FIXME: The below is ad-hoc, since we don't yet have scrollable elements.
    //        Return here and implement this according to spec once all overflow is made scrollable.

    (void)behavior;
    (void)block;
    (void)inline_;

    if (!element.document().browsing_context())
        return Error::from_string_view("Element has no browsing context."sv);

    auto* page = element.document().browsing_context()->page();
    if (!page)
        return Error::from_string_view("Element has no page."sv);

    // If this element doesn't have a layout node, we can't scroll it into view.
    element.document().update_layout();
    if (!element.layout_node())
        return Error::from_string_view("Element has no layout node."sv);

    // Find the nearest layout node that is a box (since we need a box to get a usable rect)
    auto* layout_node = element.layout_node();
    while (layout_node && !layout_node->is_box())
        layout_node = layout_node->parent();

    if (!layout_node)
        return Error::from_string_view("Element has no parent layout node that is a box."sv);

    page->client().page_did_request_scroll_into_view(verify_cast<Layout::Box>(*layout_node).paint_box()->absolute_padding_box_rect());

    return {};
}

// https://w3c.github.io/csswg-drafts/cssom-view-1/#dom-element-scrollintoview
ErrorOr<void> Element::scroll_into_view(Optional<Variant<bool, ScrollIntoViewOptions>> arg)
{
    // 1. Let behavior be "auto".
    auto behavior = Bindings::ScrollBehavior::Auto;

    // 2. Let block be "start".
    auto block = Bindings::ScrollLogicalPosition::Start;

    // 3. Let inline be "nearest".
    auto inline_ = Bindings::ScrollLogicalPosition::Nearest;

    // 4. If arg is a ScrollIntoViewOptions dictionary, then:
    if (arg.has_value() && arg->has<ScrollIntoViewOptions>()) {
        // 1. Set behavior to the behavior dictionary member of options.
        behavior = arg->get<ScrollIntoViewOptions>().behavior;

        // 2. Set block to the block dictionary member of options.
        block = arg->get<ScrollIntoViewOptions>().block;

        // 3. Set inline to the inline dictionary member of options.
        inline_ = arg->get<ScrollIntoViewOptions>().inline_;
    }
    // 5. Otherwise, if arg is false, then set block to "end".
    else if (arg.has_value() && arg->has<bool>() && arg->get<bool>() == false) {
        block = Bindings::ScrollLogicalPosition::End;
    }

    // 6. If the element does not have any associated box, or is not available to user-agent features, then return.
    document().update_layout();
    if (!layout_node())
        return Error::from_string_view("Element has no associated box"sv);

    // 7. Scroll the element into view with behavior, block, and inline.
    TRY(scroll_an_element_into_view(*this, behavior, block, inline_));

    return {};

    // FIXME: 8. Optionally perform some other action that brings the element to the user’s attention.
}

void Element::invalidate_style_after_attribute_change(FlyString const& attribute_name)
{
    // FIXME: Only invalidate if the attribute can actually affect style.
    (void)attribute_name;

    // FIXME: This will need to become smarter when we implement the :has() selector.
    invalidate_style();
}

// https://www.w3.org/TR/wai-aria-1.2/#tree_exclusion
bool Element::exclude_from_accessibility_tree() const
{
    // The following elements are not exposed via the accessibility API and user agents MUST NOT include them in the accessibility tree:

    // Elements, including their descendent elements, that have host language semantics specifying that the element is not displayed, such as CSS display:none, visibility:hidden, or the HTML hidden attribute.
    if (!layout_node())
        return true;

    // Elements with none or presentation as the first role in the role attribute. However, their exclusion is conditional. In addition, the element's descendants and text content are generally included. These exceptions and conditions are documented in the presentation (role) section.
    // FIXME: Handle exceptions to excluding presentation role
    auto role = role_or_default().to_lowercase();
    if (role == ARIARoleNames::none || role == ARIARoleNames::presentation)
        return true;

    // TODO: If not already excluded from the accessibility tree per the above rules, user agents SHOULD NOT include the following elements in the accessibility tree:
    //    Elements, including their descendants, that have aria-hidden set to true. In other words, aria-hidden="true" on a parent overrides aria-hidden="false" on descendants.
    //    Any descendants of elements that have the characteristic "Children Presentational: True" unless the descendant is not allowed to be presentational because it meets one of the conditions for exception described in Presentational Roles Conflict Resolution. However, the text content of any excluded descendants is included.
    //    Elements with the following roles have the characteristic "Children Presentational: True":
    //      button
    //      checkbox
    //      img
    //      menuitemcheckbox
    //      menuitemradio
    //      meter
    //      option
    //      progressbar
    //      radio
    //      scrollbar
    //      separator
    //      slider
    //      switch
    //      tab
    return false;
}

// https://www.w3.org/TR/wai-aria-1.2/#tree_inclusion
bool Element::include_in_accessibility_tree() const
{
    // If not excluded from or marked as hidden in the accessibility tree per the rules above in Excluding Elements in the Accessibility Tree, user agents MUST provide an accessible object in the accessibility tree for DOM elements that meet any of the following criteria:
    if (exclude_from_accessibility_tree())
        return false;
    // Elements that are not hidden and may fire an accessibility API event, including:
    // Elements that are currently focused, even if the element or one of its ancestor elements has its aria-hidden attribute set to true.
    if (is_focused())
        return true;
    // TODO: Elements that are a valid target of an aria-activedescendant attribute.

    // Elements that have an explicit role or a global WAI-ARIA attribute and do not have aria-hidden set to true. (See Excluding Elements in the Accessibility Tree for additional guidance on aria-hidden.)
    // NOTE: The spec says only explicit roles count, but playing around in other browsers, this does not seem to be true in practice (for example button elements are always exposed with their implicit role if none is set)
    //       This issue https://github.com/w3c/aria/issues/1851 seeks clarification on this point
    if ((!role_or_default().is_empty() || has_global_aria_attribute()) && aria_hidden() != "true")
        return true;

    // TODO: Elements that are not hidden and have an ID that is referenced by another element via a WAI-ARIA property.

    return false;
}

}
