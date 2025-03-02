/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021-2023, Sam Atkins <atkinssj@serenityos.org>
 * Copyright (c) 2021, Tobias Christiansen <tobyase@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteBuffer.h>
#include <LibGfx/Palette.h>
#include <LibWeb/CSS/Serialize.h>
#include <LibWeb/CSS/StyleValue.h>
#include <LibWeb/DOM/Document.h>
#include <LibWeb/HTML/BrowsingContext.h>
#include <LibWeb/Loader/LoadRequest.h>
#include <LibWeb/Loader/ResourceLoader.h>
#include <LibWeb/Page/Page.h>
#include <LibWeb/Painting/GradientPainting.h>
#include <LibWeb/Platform/Timer.h>

namespace Web::CSS {

StyleValue::StyleValue(Type type)
    : m_type(type)
{
}

AbstractImageStyleValue const& StyleValue::as_abstract_image() const
{
    VERIFY(is_abstract_image());
    return static_cast<AbstractImageStyleValue const&>(*this);
}

AngleStyleValue const& StyleValue::as_angle() const
{
    VERIFY(is_angle());
    return static_cast<AngleStyleValue const&>(*this);
}

BackgroundStyleValue const& StyleValue::as_background() const
{
    VERIFY(is_background());
    return static_cast<BackgroundStyleValue const&>(*this);
}

BackgroundRepeatStyleValue const& StyleValue::as_background_repeat() const
{
    VERIFY(is_background_repeat());
    return static_cast<BackgroundRepeatStyleValue const&>(*this);
}

BackgroundSizeStyleValue const& StyleValue::as_background_size() const
{
    VERIFY(is_background_size());
    return static_cast<BackgroundSizeStyleValue const&>(*this);
}

BorderStyleValue const& StyleValue::as_border() const
{
    VERIFY(is_border());
    return static_cast<BorderStyleValue const&>(*this);
}

BorderRadiusStyleValue const& StyleValue::as_border_radius() const
{
    VERIFY(is_border_radius());
    return static_cast<BorderRadiusStyleValue const&>(*this);
}

BorderRadiusShorthandStyleValue const& StyleValue::as_border_radius_shorthand() const
{
    VERIFY(is_border_radius_shorthand());
    return static_cast<BorderRadiusShorthandStyleValue const&>(*this);
}

ShadowStyleValue const& StyleValue::as_shadow() const
{
    VERIFY(is_shadow());
    return static_cast<ShadowStyleValue const&>(*this);
}

CalculatedStyleValue const& StyleValue::as_calculated() const
{
    VERIFY(is_calculated());
    return static_cast<CalculatedStyleValue const&>(*this);
}

ColorStyleValue const& StyleValue::as_color() const
{
    VERIFY(is_color());
    return static_cast<ColorStyleValue const&>(*this);
}

ConicGradientStyleValue const& StyleValue::as_conic_gradient() const
{
    VERIFY(is_conic_gradient());
    return static_cast<ConicGradientStyleValue const&>(*this);
}

ContentStyleValue const& StyleValue::as_content() const
{
    VERIFY(is_content());
    return static_cast<ContentStyleValue const&>(*this);
}

FilterValueListStyleValue const& StyleValue::as_filter_value_list() const
{
    VERIFY(is_filter_value_list());
    return static_cast<FilterValueListStyleValue const&>(*this);
}

FlexStyleValue const& StyleValue::as_flex() const
{
    VERIFY(is_flex());
    return static_cast<FlexStyleValue const&>(*this);
}

FlexFlowStyleValue const& StyleValue::as_flex_flow() const
{
    VERIFY(is_flex_flow());
    return static_cast<FlexFlowStyleValue const&>(*this);
}

FontStyleValue const& StyleValue::as_font() const
{
    VERIFY(is_font());
    return static_cast<FontStyleValue const&>(*this);
}

FrequencyStyleValue const& StyleValue::as_frequency() const
{
    VERIFY(is_frequency());
    return static_cast<FrequencyStyleValue const&>(*this);
}

GridTrackPlacementShorthandStyleValue const& StyleValue::as_grid_track_placement_shorthand() const
{
    VERIFY(is_grid_track_placement_shorthand());
    return static_cast<GridTrackPlacementShorthandStyleValue const&>(*this);
}

GridTrackPlacementStyleValue const& StyleValue::as_grid_track_placement() const
{
    VERIFY(is_grid_track_placement());
    return static_cast<GridTrackPlacementStyleValue const&>(*this);
}

IdentifierStyleValue const& StyleValue::as_identifier() const
{
    VERIFY(is_identifier());
    return static_cast<IdentifierStyleValue const&>(*this);
}

ImageStyleValue const& StyleValue::as_image() const
{
    VERIFY(is_image());
    return static_cast<ImageStyleValue const&>(*this);
}

InheritStyleValue const& StyleValue::as_inherit() const
{
    VERIFY(is_inherit());
    return static_cast<InheritStyleValue const&>(*this);
}

InitialStyleValue const& StyleValue::as_initial() const
{
    VERIFY(is_initial());
    return static_cast<InitialStyleValue const&>(*this);
}

LengthStyleValue const& StyleValue::as_length() const
{
    VERIFY(is_length());
    return static_cast<LengthStyleValue const&>(*this);
}

GridTrackSizeStyleValue const& StyleValue::as_grid_track_size_list() const
{
    VERIFY(is_grid_track_size_list());
    return static_cast<GridTrackSizeStyleValue const&>(*this);
}

LinearGradientStyleValue const& StyleValue::as_linear_gradient() const
{
    VERIFY(is_linear_gradient());
    return static_cast<LinearGradientStyleValue const&>(*this);
}

ListStyleStyleValue const& StyleValue::as_list_style() const
{
    VERIFY(is_list_style());
    return static_cast<ListStyleStyleValue const&>(*this);
}

NumericStyleValue const& StyleValue::as_numeric() const
{
    VERIFY(is_numeric());
    return static_cast<NumericStyleValue const&>(*this);
}

OverflowStyleValue const& StyleValue::as_overflow() const
{
    VERIFY(is_overflow());
    return static_cast<OverflowStyleValue const&>(*this);
}

PercentageStyleValue const& StyleValue::as_percentage() const
{
    VERIFY(is_percentage());
    return static_cast<PercentageStyleValue const&>(*this);
}

PositionStyleValue const& StyleValue::as_position() const
{
    VERIFY(is_position());
    return static_cast<PositionStyleValue const&>(*this);
}

RadialGradientStyleValue const& StyleValue::as_radial_gradient() const
{
    VERIFY(is_radial_gradient());
    return static_cast<RadialGradientStyleValue const&>(*this);
}

RectStyleValue const& StyleValue::as_rect() const
{
    VERIFY(is_rect());
    return static_cast<RectStyleValue const&>(*this);
}

ResolutionStyleValue const& StyleValue::as_resolution() const
{
    VERIFY(is_resolution());
    return static_cast<ResolutionStyleValue const&>(*this);
}

StringStyleValue const& StyleValue::as_string() const
{
    VERIFY(is_string());
    return static_cast<StringStyleValue const&>(*this);
}

TextDecorationStyleValue const& StyleValue::as_text_decoration() const
{
    VERIFY(is_text_decoration());
    return static_cast<TextDecorationStyleValue const&>(*this);
}

TimeStyleValue const& StyleValue::as_time() const
{
    VERIFY(is_time());
    return static_cast<TimeStyleValue const&>(*this);
}

TransformationStyleValue const& StyleValue::as_transformation() const
{
    VERIFY(is_transformation());
    return static_cast<TransformationStyleValue const&>(*this);
}

UnresolvedStyleValue const& StyleValue::as_unresolved() const
{
    VERIFY(is_unresolved());
    return static_cast<UnresolvedStyleValue const&>(*this);
}

UnsetStyleValue const& StyleValue::as_unset() const
{
    VERIFY(is_unset());
    return static_cast<UnsetStyleValue const&>(*this);
}

StyleValueList const& StyleValue::as_value_list() const
{
    VERIFY(is_value_list());
    return static_cast<StyleValueList const&>(*this);
}

BackgroundStyleValue::BackgroundStyleValue(
    NonnullRefPtr<StyleValue> color,
    NonnullRefPtr<StyleValue> image,
    NonnullRefPtr<StyleValue> position,
    NonnullRefPtr<StyleValue> size,
    NonnullRefPtr<StyleValue> repeat,
    NonnullRefPtr<StyleValue> attachment,
    NonnullRefPtr<StyleValue> origin,
    NonnullRefPtr<StyleValue> clip)
    : StyleValue(Type::Background)
    , m_color(color)
    , m_image(image)
    , m_position(position)
    , m_size(size)
    , m_repeat(repeat)
    , m_attachment(attachment)
    , m_origin(origin)
    , m_clip(clip)
{
    auto layer_count = [](auto style_value) -> size_t {
        if (style_value->is_value_list())
            return style_value->as_value_list().size();
        else
            return 1;
    };

    m_layer_count = max(layer_count(m_image), layer_count(m_position));
    m_layer_count = max(m_layer_count, layer_count(m_size));
    m_layer_count = max(m_layer_count, layer_count(m_repeat));
    m_layer_count = max(m_layer_count, layer_count(m_attachment));
    m_layer_count = max(m_layer_count, layer_count(m_origin));
    m_layer_count = max(m_layer_count, layer_count(m_clip));

    VERIFY(!m_color->is_value_list());
}

ErrorOr<String> BackgroundStyleValue::to_string() const
{
    if (m_layer_count == 1) {
        return String::formatted("{} {} {} {} {} {} {} {}", TRY(m_color->to_string()), TRY(m_image->to_string()), TRY(m_position->to_string()), TRY(m_size->to_string()), TRY(m_repeat->to_string()), TRY(m_attachment->to_string()), TRY(m_origin->to_string()), TRY(m_clip->to_string()));
    }

    auto get_layer_value_string = [](NonnullRefPtr<StyleValue> const& style_value, size_t index) {
        if (style_value->is_value_list())
            return style_value->as_value_list().value_at(index, true)->to_string();
        return style_value->to_string();
    };

    StringBuilder builder;
    for (size_t i = 0; i < m_layer_count; i++) {
        if (i)
            builder.append(", "sv);
        if (i == m_layer_count - 1)
            builder.appendff("{} ", TRY(m_color->to_string()));
        builder.appendff("{} {} {} {} {} {} {}", TRY(get_layer_value_string(m_image, i)), TRY(get_layer_value_string(m_position, i)), TRY(get_layer_value_string(m_size, i)), TRY(get_layer_value_string(m_repeat, i)), TRY(get_layer_value_string(m_attachment, i)), TRY(get_layer_value_string(m_origin, i)), TRY(get_layer_value_string(m_clip, i)));
    }

    return builder.to_string();
}

bool BackgroundStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_background();
    return m_color->equals(typed_other.m_color)
        && m_image->equals(typed_other.m_image)
        && m_position->equals(typed_other.m_position)
        && m_size->equals(typed_other.m_size)
        && m_repeat->equals(typed_other.m_repeat)
        && m_attachment->equals(typed_other.m_attachment)
        && m_origin->equals(typed_other.m_origin)
        && m_clip->equals(typed_other.m_clip);
}

ErrorOr<String> BackgroundRepeatStyleValue::to_string() const
{
    return String::formatted("{} {}", CSS::to_string(m_repeat_x), CSS::to_string(m_repeat_y));
}

bool BackgroundRepeatStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_background_repeat();
    return m_repeat_x == typed_other.m_repeat_x && m_repeat_y == typed_other.m_repeat_y;
}

ErrorOr<String> BackgroundSizeStyleValue::to_string() const
{
    return String::formatted("{} {}", TRY(m_size_x.to_string()), TRY(m_size_y.to_string()));
}

bool BackgroundSizeStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_background_size();
    return m_size_x == typed_other.m_size_x && m_size_y == typed_other.m_size_y;
}

ErrorOr<String> BorderStyleValue::to_string() const
{
    return String::formatted("{} {} {}", TRY(m_border_width->to_string()), TRY(m_border_style->to_string()), TRY(m_border_color->to_string()));
}

bool BorderStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_border();
    return m_border_width->equals(typed_other.m_border_width)
        && m_border_style->equals(typed_other.m_border_style)
        && m_border_color->equals(typed_other.m_border_color);
}

ErrorOr<String> BorderRadiusStyleValue::to_string() const
{
    if (m_horizontal_radius == m_vertical_radius)
        return m_horizontal_radius.to_string();
    return String::formatted("{} / {}", TRY(m_horizontal_radius.to_string()), TRY(m_vertical_radius.to_string()));
}

bool BorderRadiusStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_border_radius();
    return m_is_elliptical == typed_other.m_is_elliptical
        && m_horizontal_radius == typed_other.m_horizontal_radius
        && m_vertical_radius == typed_other.m_vertical_radius;
}

ErrorOr<String> BorderRadiusShorthandStyleValue::to_string() const
{
    return String::formatted("{} {} {} {} / {} {} {} {}", TRY(m_top_left->horizontal_radius().to_string()), TRY(m_top_right->horizontal_radius().to_string()), TRY(m_bottom_right->horizontal_radius().to_string()), TRY(m_bottom_left->horizontal_radius().to_string()), TRY(m_top_left->vertical_radius().to_string()), TRY(m_top_right->vertical_radius().to_string()), TRY(m_bottom_right->vertical_radius().to_string()), TRY(m_bottom_left->vertical_radius().to_string()));
}

bool BorderRadiusShorthandStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_border_radius_shorthand();
    return m_top_left->equals(typed_other.m_top_left)
        && m_top_right->equals(typed_other.m_top_right)
        && m_bottom_right->equals(typed_other.m_bottom_right)
        && m_bottom_left->equals(typed_other.m_bottom_left);
}

void CalculatedStyleValue::CalculationResult::add(CalculationResult const& other, Layout::Node const* layout_node, PercentageBasis const& percentage_basis)
{
    add_or_subtract_internal(SumOperation::Add, other, layout_node, percentage_basis);
}

void CalculatedStyleValue::CalculationResult::subtract(CalculationResult const& other, Layout::Node const* layout_node, PercentageBasis const& percentage_basis)
{
    add_or_subtract_internal(SumOperation::Subtract, other, layout_node, percentage_basis);
}

void CalculatedStyleValue::CalculationResult::add_or_subtract_internal(SumOperation op, CalculationResult const& other, Layout::Node const* layout_node, PercentageBasis const& percentage_basis)
{
    // We know from validation when resolving the type, that "both sides have the same type, or that one side is a <number> and the other is an <integer>".
    // Though, having the same type may mean that one side is a <dimension> and the other a <percentage>.
    // Note: This is almost identical to ::add()

    m_value.visit(
        [&](Number const& number) {
            auto other_number = other.m_value.get<Number>();
            if (op == SumOperation::Add) {
                m_value = number + other_number;
            } else {
                m_value = number - other_number;
            }
        },
        [&](Angle const& angle) {
            auto this_degrees = angle.to_degrees();
            if (other.m_value.has<Angle>()) {
                auto other_degrees = other.m_value.get<Angle>().to_degrees();
                if (op == SumOperation::Add)
                    m_value = Angle::make_degrees(this_degrees + other_degrees);
                else
                    m_value = Angle::make_degrees(this_degrees - other_degrees);
            } else {
                VERIFY(percentage_basis.has<Angle>());

                auto other_degrees = percentage_basis.get<Angle>().percentage_of(other.m_value.get<Percentage>()).to_degrees();
                if (op == SumOperation::Add)
                    m_value = Angle::make_degrees(this_degrees + other_degrees);
                else
                    m_value = Angle::make_degrees(this_degrees - other_degrees);
            }
        },
        [&](Frequency const& frequency) {
            auto this_hertz = frequency.to_hertz();
            if (other.m_value.has<Frequency>()) {
                auto other_hertz = other.m_value.get<Frequency>().to_hertz();
                if (op == SumOperation::Add)
                    m_value = Frequency::make_hertz(this_hertz + other_hertz);
                else
                    m_value = Frequency::make_hertz(this_hertz - other_hertz);
            } else {
                VERIFY(percentage_basis.has<Frequency>());

                auto other_hertz = percentage_basis.get<Frequency>().percentage_of(other.m_value.get<Percentage>()).to_hertz();
                if (op == SumOperation::Add)
                    m_value = Frequency::make_hertz(this_hertz + other_hertz);
                else
                    m_value = Frequency::make_hertz(this_hertz - other_hertz);
            }
        },
        [&](Length const& length) {
            auto this_px = length.to_px(*layout_node);
            if (other.m_value.has<Length>()) {
                auto other_px = other.m_value.get<Length>().to_px(*layout_node);
                if (op == SumOperation::Add)
                    m_value = Length::make_px(this_px + other_px);
                else
                    m_value = Length::make_px(this_px - other_px);
            } else {
                VERIFY(percentage_basis.has<Length>());

                auto other_px = percentage_basis.get<Length>().percentage_of(other.m_value.get<Percentage>()).to_px(*layout_node);
                if (op == SumOperation::Add)
                    m_value = Length::make_px(this_px + other_px);
                else
                    m_value = Length::make_px(this_px - other_px);
            }
        },
        [&](Time const& time) {
            auto this_seconds = time.to_seconds();
            if (other.m_value.has<Time>()) {
                auto other_seconds = other.m_value.get<Time>().to_seconds();
                if (op == SumOperation::Add)
                    m_value = Time::make_seconds(this_seconds + other_seconds);
                else
                    m_value = Time::make_seconds(this_seconds - other_seconds);
            } else {
                VERIFY(percentage_basis.has<Time>());

                auto other_seconds = percentage_basis.get<Time>().percentage_of(other.m_value.get<Percentage>()).to_seconds();
                if (op == SumOperation::Add)
                    m_value = Time::make_seconds(this_seconds + other_seconds);
                else
                    m_value = Time::make_seconds(this_seconds - other_seconds);
            }
        },
        [&](Percentage const& percentage) {
            if (other.m_value.has<Percentage>()) {
                if (op == SumOperation::Add)
                    m_value = Percentage { percentage.value() + other.m_value.get<Percentage>().value() };
                else
                    m_value = Percentage { percentage.value() - other.m_value.get<Percentage>().value() };
                return;
            }

            // Other side isn't a percentage, so the easiest way to handle it without duplicating all the logic, is just to swap `this` and `other`.
            CalculationResult new_value = other;
            if (op == SumOperation::Add) {
                new_value.add(*this, layout_node, percentage_basis);
            } else {
                // Turn 'this - other' into '-other + this', as 'A + B == B + A', but 'A - B != B - A'
                new_value.multiply_by({ Number { Number::Type::Integer, -1.0f } }, layout_node);
                new_value.add(*this, layout_node, percentage_basis);
            }

            *this = new_value;
        });
}

void CalculatedStyleValue::CalculationResult::multiply_by(CalculationResult const& other, Layout::Node const* layout_node)
{
    // We know from validation when resolving the type, that at least one side must be a <number> or <integer>.
    // Both of these are represented as a float.
    VERIFY(m_value.has<Number>() || other.m_value.has<Number>());
    bool other_is_number = other.m_value.has<Number>();

    m_value.visit(
        [&](Number const& number) {
            if (other_is_number) {
                m_value = number * other.m_value.get<Number>();
            } else {
                // Avoid duplicating all the logic by swapping `this` and `other`.
                CalculationResult new_value = other;
                new_value.multiply_by(*this, layout_node);
                *this = new_value;
            }
        },
        [&](Angle const& angle) {
            m_value = Angle::make_degrees(angle.to_degrees() * other.m_value.get<Number>().value());
        },
        [&](Frequency const& frequency) {
            m_value = Frequency::make_hertz(frequency.to_hertz() * other.m_value.get<Number>().value());
        },
        [&](Length const& length) {
            VERIFY(layout_node);
            m_value = Length::make_px(length.to_px(*layout_node) * other.m_value.get<Number>().value());
        },
        [&](Time const& time) {
            m_value = Time::make_seconds(time.to_seconds() * other.m_value.get<Number>().value());
        },
        [&](Percentage const& percentage) {
            m_value = Percentage { percentage.value() * other.m_value.get<Number>().value() };
        });
}

void CalculatedStyleValue::CalculationResult::divide_by(CalculationResult const& other, Layout::Node const* layout_node)
{
    // We know from validation when resolving the type, that `other` must be a <number> or <integer>.
    // Both of these are represented as a Number.
    auto denominator = other.m_value.get<Number>().value();
    // FIXME: Dividing by 0 is invalid, and should be caught during parsing.
    VERIFY(denominator != 0.0f);

    m_value.visit(
        [&](Number const& number) {
            m_value = Number {
                Number::Type::Number,
                number.value() / denominator
            };
        },
        [&](Angle const& angle) {
            m_value = Angle::make_degrees(angle.to_degrees() / denominator);
        },
        [&](Frequency const& frequency) {
            m_value = Frequency::make_hertz(frequency.to_hertz() / denominator);
        },
        [&](Length const& length) {
            VERIFY(layout_node);
            m_value = Length::make_px(length.to_px(*layout_node) / denominator);
        },
        [&](Time const& time) {
            m_value = Time::make_seconds(time.to_seconds() / denominator);
        },
        [&](Percentage const& percentage) {
            m_value = Percentage { percentage.value() / denominator };
        });
}

ErrorOr<String> CalculatedStyleValue::to_string() const
{
    return String::formatted("calc({})", TRY(m_expression->to_string()));
}

bool CalculatedStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    // This is a case where comparing the strings actually makes sense.
    return to_string().release_value_but_fixme_should_propagate_errors() == other.to_string().release_value_but_fixme_should_propagate_errors();
}

ErrorOr<String> CalculatedStyleValue::CalcNumberValue::to_string() const
{
    return value.visit(
        [](Number const& number) { return String::number(number.value()); },
        [](NonnullOwnPtr<CalcNumberSum> const& sum) { return String::formatted("({})", sum->to_string()); });
}

ErrorOr<String> CalculatedStyleValue::CalcValue::to_string() const
{
    return value.visit(
        [](Number const& number) { return String::number(number.value()); },
        [](NonnullOwnPtr<CalcSum> const& sum) { return String::formatted("({})", sum->to_string()); },
        [](auto const& v) { return v.to_string(); });
}

ErrorOr<String> CalculatedStyleValue::CalcSum::to_string() const
{
    StringBuilder builder;
    builder.append(TRY(first_calc_product->to_string()));
    for (auto const& item : zero_or_more_additional_calc_products)
        builder.append(TRY(item.to_string()));
    return builder.to_string();
}

ErrorOr<String> CalculatedStyleValue::CalcNumberSum::to_string() const
{
    StringBuilder builder;
    builder.append(TRY(first_calc_number_product->to_string()));
    for (auto const& item : zero_or_more_additional_calc_number_products)
        builder.append(TRY(item.to_string()));
    return builder.to_string();
}

ErrorOr<String> CalculatedStyleValue::CalcProduct::to_string() const
{
    StringBuilder builder;
    builder.append(TRY(first_calc_value.to_string()));
    for (auto const& item : zero_or_more_additional_calc_values)
        builder.append(TRY(item.to_string()));
    return builder.to_string();
}

ErrorOr<String> CalculatedStyleValue::CalcSumPartWithOperator::to_string() const
{
    return String::formatted(" {} {}", op == SumOperation::Add ? "+"sv : "-"sv, TRY(value->to_string()));
}

ErrorOr<String> CalculatedStyleValue::CalcProductPartWithOperator::to_string() const
{
    auto value_string = value.visit(
        [](CalcValue const& v) { return v.to_string(); },
        [](CalcNumberValue const& v) { return v.to_string(); });
    return String::formatted(" {} {}", op == ProductOperation::Multiply ? "*"sv : "/"sv, value_string);
}

ErrorOr<String> CalculatedStyleValue::CalcNumberProduct::to_string() const
{
    StringBuilder builder;
    builder.append(TRY(first_calc_number_value.to_string()));
    for (auto const& item : zero_or_more_additional_calc_number_values)
        builder.append(TRY(item.to_string()));
    return builder.to_string();
}

ErrorOr<String> CalculatedStyleValue::CalcNumberProductPartWithOperator::to_string() const
{
    return String::formatted(" {} {}", op == ProductOperation::Multiply ? "*"sv : "/"sv, TRY(value.to_string()));
}

ErrorOr<String> CalculatedStyleValue::CalcNumberSumPartWithOperator::to_string() const
{
    return String::formatted(" {} {}", op == SumOperation::Add ? "+"sv : "-"sv, TRY(value->to_string()));
}

Optional<Angle> CalculatedStyleValue::resolve_angle() const
{
    auto result = m_expression->resolve(nullptr, {});

    if (result.value().has<Angle>())
        return result.value().get<Angle>();
    return {};
}

Optional<Angle> CalculatedStyleValue::resolve_angle_percentage(Angle const& percentage_basis) const
{
    auto result = m_expression->resolve(nullptr, percentage_basis);

    return result.value().visit(
        [&](Angle const& angle) -> Optional<Angle> {
            return angle;
        },
        [&](Percentage const& percentage) -> Optional<Angle> {
            return percentage_basis.percentage_of(percentage);
        },
        [&](auto const&) -> Optional<Angle> {
            return {};
        });
}

Optional<Frequency> CalculatedStyleValue::resolve_frequency() const
{
    auto result = m_expression->resolve(nullptr, {});

    if (result.value().has<Frequency>())
        return result.value().get<Frequency>();
    return {};
}

Optional<Frequency> CalculatedStyleValue::resolve_frequency_percentage(Frequency const& percentage_basis) const
{
    auto result = m_expression->resolve(nullptr, percentage_basis);

    return result.value().visit(
        [&](Frequency const& frequency) -> Optional<Frequency> {
            return frequency;
        },
        [&](Percentage const& percentage) -> Optional<Frequency> {
            return percentage_basis.percentage_of(percentage);
        },
        [&](auto const&) -> Optional<Frequency> {
            return {};
        });
}

Optional<Length> CalculatedStyleValue::resolve_length(Layout::Node const& layout_node) const
{
    auto result = m_expression->resolve(&layout_node, {});

    if (result.value().has<Length>())
        return result.value().get<Length>();
    return {};
}

Optional<Length> CalculatedStyleValue::resolve_length_percentage(Layout::Node const& layout_node, Length const& percentage_basis) const
{
    auto result = m_expression->resolve(&layout_node, percentage_basis);

    return result.value().visit(
        [&](Length const& length) -> Optional<Length> {
            return length;
        },
        [&](Percentage const& percentage) -> Optional<Length> {
            return percentage_basis.percentage_of(percentage);
        },
        [&](auto const&) -> Optional<Length> {
            return {};
        });
}

Optional<Percentage> CalculatedStyleValue::resolve_percentage() const
{
    auto result = m_expression->resolve(nullptr, {});
    if (result.value().has<Percentage>())
        return result.value().get<Percentage>();
    return {};
}

Optional<Time> CalculatedStyleValue::resolve_time() const
{
    auto result = m_expression->resolve(nullptr, {});

    if (result.value().has<Time>())
        return result.value().get<Time>();
    return {};
}

Optional<Time> CalculatedStyleValue::resolve_time_percentage(Time const& percentage_basis) const
{
    auto result = m_expression->resolve(nullptr, percentage_basis);

    return result.value().visit(
        [&](Time const& time) -> Optional<Time> {
            return time;
        },
        [&](auto const&) -> Optional<Time> {
            return {};
        });
}

Optional<float> CalculatedStyleValue::resolve_number()
{
    auto result = m_expression->resolve(nullptr, {});
    if (result.value().has<Number>())
        return result.value().get<Number>().value();
    return {};
}

Optional<i64> CalculatedStyleValue::resolve_integer()
{
    auto result = m_expression->resolve(nullptr, {});
    if (result.value().has<Number>())
        return result.value().get<Number>().integer_value();
    return {};
}

static bool is_number(CalculatedStyleValue::ResolvedType type)
{
    return type == CalculatedStyleValue::ResolvedType::Number || type == CalculatedStyleValue::ResolvedType::Integer;
}

static bool is_dimension(CalculatedStyleValue::ResolvedType type)
{
    return type != CalculatedStyleValue::ResolvedType::Number
        && type != CalculatedStyleValue::ResolvedType::Integer
        && type != CalculatedStyleValue::ResolvedType::Percentage;
}

template<typename SumWithOperator>
static Optional<CalculatedStyleValue::ResolvedType> resolve_sum_type(CalculatedStyleValue::ResolvedType first_type, NonnullOwnPtrVector<SumWithOperator> const& zero_or_more_additional_products)
{
    auto type = first_type;

    for (auto const& product : zero_or_more_additional_products) {
        auto maybe_product_type = product.resolved_type();
        if (!maybe_product_type.has_value())
            return {};
        auto product_type = maybe_product_type.value();

        // At + or -, check that both sides have the same type, or that one side is a <number> and the other is an <integer>.
        // If both sides are the same type, resolve to that type.
        if (product_type == type)
            continue;

        // If one side is a <number> and the other is an <integer>, resolve to <number>.
        if (is_number(type) && is_number(product_type)) {
            type = CalculatedStyleValue::ResolvedType::Number;
            continue;
        }

        // FIXME: calc() handles <percentage> by allowing them to pretend to be whatever <dimension> type is allowed at this location.
        //        Since we can't easily check what that type is, we just allow <percentage> to combine with any other <dimension> type.
        if (type == CalculatedStyleValue::ResolvedType::Percentage && is_dimension(product_type)) {
            type = product_type;
            continue;
        }
        if (is_dimension(type) && product_type == CalculatedStyleValue::ResolvedType::Percentage)
            continue;

        return {};
    }
    return type;
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcSum::resolved_type() const
{
    auto maybe_type = first_calc_product->resolved_type();
    if (!maybe_type.has_value())
        return {};
    auto type = maybe_type.value();
    return resolve_sum_type(type, zero_or_more_additional_calc_products);
}

// https://www.w3.org/TR/CSS2/visufx.html#value-def-shape
Gfx::FloatRect EdgeRect::resolved(Layout::Node const& layout_node, Gfx::FloatRect border_box) const
{
    // In CSS 2.1, the only valid <shape> value is: rect(<top>, <right>, <bottom>, <left>) where
    // <top> and <bottom> specify offsets from the top border edge of the box, and <right>, and
    // <left> specify offsets from the left border edge of the box.

    // The value 'auto' means that a given edge of the clipping region will be the same as the edge
    // of the element's generated border box (i.e., 'auto' means the same as '0' for <top> and
    // <left>, the same as the used value of the height plus the sum of vertical padding and border
    // widths for <bottom>, and the same as the used value of the width plus the sum of the
    // horizontal padding and border widths for <right>, such that four 'auto' values result in the
    // clipping region being the same as the element's border box).
    auto left = border_box.left() + (left_edge.is_auto() ? 0 : left_edge.to_px(layout_node)).value();
    auto top = border_box.top() + (top_edge.is_auto() ? 0 : top_edge.to_px(layout_node)).value();
    auto right = border_box.left() + (right_edge.is_auto() ? border_box.width() : right_edge.to_px(layout_node)).value();
    auto bottom = border_box.top() + (bottom_edge.is_auto() ? border_box.height() : bottom_edge.to_px(layout_node)).value();
    return Gfx::FloatRect {
        left,
        top,
        right - left,
        bottom - top
    };
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcNumberSum::resolved_type() const
{
    auto maybe_type = first_calc_number_product->resolved_type();
    if (!maybe_type.has_value())
        return {};
    auto type = maybe_type.value();
    return resolve_sum_type(type, zero_or_more_additional_calc_number_products);
}

template<typename ProductWithOperator>
static Optional<CalculatedStyleValue::ResolvedType> resolve_product_type(CalculatedStyleValue::ResolvedType first_type, NonnullOwnPtrVector<ProductWithOperator> const& zero_or_more_additional_values)
{
    auto type = first_type;

    for (auto const& value : zero_or_more_additional_values) {
        auto maybe_value_type = value.resolved_type();
        if (!maybe_value_type.has_value())
            return {};
        auto value_type = maybe_value_type.value();

        if (value.op == CalculatedStyleValue::ProductOperation::Multiply) {
            // At *, check that at least one side is <number>.
            if (!(is_number(type) || is_number(value_type)))
                return {};
            // If both sides are <integer>, resolve to <integer>.
            if (type == CalculatedStyleValue::ResolvedType::Integer && value_type == CalculatedStyleValue::ResolvedType::Integer) {
                type = CalculatedStyleValue::ResolvedType::Integer;
            } else {
                // Otherwise, resolve to the type of the other side.
                if (is_number(type))
                    type = value_type;
            }

            continue;
        } else {
            VERIFY(value.op == CalculatedStyleValue::ProductOperation::Divide);
            // At /, check that the right side is <number>.
            if (!is_number(value_type))
                return {};
            // If the left side is <integer>, resolve to <number>.
            if (type == CalculatedStyleValue::ResolvedType::Integer) {
                type = CalculatedStyleValue::ResolvedType::Number;
            } else {
                // Otherwise, resolve to the type of the left side.
            }

            // FIXME: Division by zero makes the whole calc() expression invalid.
        }
    }
    return type;
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcProduct::resolved_type() const
{
    auto maybe_type = first_calc_value.resolved_type();
    if (!maybe_type.has_value())
        return {};
    auto type = maybe_type.value();
    return resolve_product_type(type, zero_or_more_additional_calc_values);
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcSumPartWithOperator::resolved_type() const
{
    return value->resolved_type();
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcNumberProduct::resolved_type() const
{
    auto maybe_type = first_calc_number_value.resolved_type();
    if (!maybe_type.has_value())
        return {};
    auto type = maybe_type.value();
    return resolve_product_type(type, zero_or_more_additional_calc_number_values);
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcNumberProductPartWithOperator::resolved_type() const
{
    return value.resolved_type();
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcNumberSumPartWithOperator::resolved_type() const
{
    return value->resolved_type();
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcProductPartWithOperator::resolved_type() const
{
    return value.visit(
        [](CalcValue const& calc_value) {
            return calc_value.resolved_type();
        },
        [](CalcNumberValue const& calc_number_value) {
            return calc_number_value.resolved_type();
        });
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcValue::resolved_type() const
{
    return value.visit(
        [](Number const& number) -> Optional<CalculatedStyleValue::ResolvedType> {
            return { number.is_integer() ? ResolvedType::Integer : ResolvedType::Number };
        },
        [](Angle const&) -> Optional<CalculatedStyleValue::ResolvedType> { return { ResolvedType::Angle }; },
        [](Frequency const&) -> Optional<CalculatedStyleValue::ResolvedType> { return { ResolvedType::Frequency }; },
        [](Length const&) -> Optional<CalculatedStyleValue::ResolvedType> { return { ResolvedType::Length }; },
        [](Percentage const&) -> Optional<CalculatedStyleValue::ResolvedType> { return { ResolvedType::Percentage }; },
        [](Time const&) -> Optional<CalculatedStyleValue::ResolvedType> { return { ResolvedType::Time }; },
        [](NonnullOwnPtr<CalcSum> const& sum) { return sum->resolved_type(); });
}

Optional<CalculatedStyleValue::ResolvedType> CalculatedStyleValue::CalcNumberValue::resolved_type() const
{
    return value.visit(
        [](Number const& number) -> Optional<CalculatedStyleValue::ResolvedType> {
            return { number.is_integer() ? ResolvedType::Integer : ResolvedType::Number };
        },
        [](NonnullOwnPtr<CalcNumberSum> const& sum) { return sum->resolved_type(); });
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcNumberValue::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value.visit(
        [&](Number const& number) -> CalculatedStyleValue::CalculationResult {
            return CalculatedStyleValue::CalculationResult { number };
        },
        [&](NonnullOwnPtr<CalcNumberSum> const& sum) -> CalculatedStyleValue::CalculationResult {
            return sum->resolve(layout_node, percentage_basis);
        });
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcValue::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value.visit(
        [&](NonnullOwnPtr<CalcSum> const& sum) -> CalculatedStyleValue::CalculationResult {
            return sum->resolve(layout_node, percentage_basis);
        },
        [&](auto const& v) -> CalculatedStyleValue::CalculationResult {
            return CalculatedStyleValue::CalculationResult { v };
        });
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcSum::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    auto value = first_calc_product->resolve(layout_node, percentage_basis);

    for (auto& additional_product : zero_or_more_additional_calc_products) {
        auto additional_value = additional_product.resolve(layout_node, percentage_basis);

        if (additional_product.op == CalculatedStyleValue::SumOperation::Add)
            value.add(additional_value, layout_node, percentage_basis);
        else if (additional_product.op == CalculatedStyleValue::SumOperation::Subtract)
            value.subtract(additional_value, layout_node, percentage_basis);
        else
            VERIFY_NOT_REACHED();
    }

    return value;
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcNumberSum::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    auto value = first_calc_number_product->resolve(layout_node, percentage_basis);

    for (auto& additional_product : zero_or_more_additional_calc_number_products) {
        auto additional_value = additional_product.resolve(layout_node, percentage_basis);

        if (additional_product.op == CSS::CalculatedStyleValue::SumOperation::Add)
            value.add(additional_value, layout_node, percentage_basis);
        else if (additional_product.op == CalculatedStyleValue::SumOperation::Subtract)
            value.subtract(additional_value, layout_node, percentage_basis);
        else
            VERIFY_NOT_REACHED();
    }

    return value;
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcProduct::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    auto value = first_calc_value.resolve(layout_node, percentage_basis);

    for (auto& additional_value : zero_or_more_additional_calc_values) {
        additional_value.value.visit(
            [&](CalculatedStyleValue::CalcValue const& calc_value) {
                VERIFY(additional_value.op == CalculatedStyleValue::ProductOperation::Multiply);
                auto resolved_value = calc_value.resolve(layout_node, percentage_basis);
                value.multiply_by(resolved_value, layout_node);
            },
            [&](CalculatedStyleValue::CalcNumberValue const& calc_number_value) {
                VERIFY(additional_value.op == CalculatedStyleValue::ProductOperation::Divide);
                auto resolved_calc_number_value = calc_number_value.resolve(layout_node, percentage_basis);
                // FIXME: Checking for division by 0 should happen during parsing.
                VERIFY(resolved_calc_number_value.value().get<Number>().value() != 0.0f);
                value.divide_by(resolved_calc_number_value, layout_node);
            });
    }

    return value;
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcNumberProduct::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    auto value = first_calc_number_value.resolve(layout_node, percentage_basis);

    for (auto& additional_number_value : zero_or_more_additional_calc_number_values) {
        auto additional_value = additional_number_value.resolve(layout_node, percentage_basis);

        if (additional_number_value.op == CalculatedStyleValue::ProductOperation::Multiply)
            value.multiply_by(additional_value, layout_node);
        else if (additional_number_value.op == CalculatedStyleValue::ProductOperation::Divide)
            value.divide_by(additional_value, layout_node);
        else
            VERIFY_NOT_REACHED();
    }

    return value;
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcProductPartWithOperator::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value.visit(
        [&](CalcValue const& calc_value) {
            return calc_value.resolve(layout_node, percentage_basis);
        },
        [&](CalcNumberValue const& calc_number_value) {
            return calc_number_value.resolve(layout_node, percentage_basis);
        });
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcSumPartWithOperator::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value->resolve(layout_node, percentage_basis);
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcNumberProductPartWithOperator::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value.resolve(layout_node, percentage_basis);
}

CalculatedStyleValue::CalculationResult CalculatedStyleValue::CalcNumberSumPartWithOperator::resolve(Layout::Node const* layout_node, PercentageBasis const& percentage_basis) const
{
    return value->resolve(layout_node, percentage_basis);
}

ErrorOr<String> ColorStyleValue::to_string() const
{
    return serialize_a_srgb_value(m_color);
}

bool ColorStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_color == other.as_color().m_color;
}

ErrorOr<String> ContentStyleValue::to_string() const
{
    if (has_alt_text())
        return String::formatted("{} / {}", TRY(m_content->to_string()), TRY(m_alt_text->to_string()));
    return m_content->to_string();
}

bool ContentStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_content();
    if (!m_content->equals(typed_other.m_content))
        return false;
    if (m_alt_text.is_null() != typed_other.m_alt_text.is_null())
        return false;
    if (!m_alt_text.is_null())
        return m_alt_text->equals(*typed_other.m_alt_text);
    return true;
}

float Filter::Blur::resolved_radius(Layout::Node const& node) const
{
    // Default value when omitted is 0px.
    auto sigma = 0;
    if (radius.has_value())
        sigma = radius->resolved(node).to_px(node).value();
    // Note: The radius/sigma of the blur needs to be doubled for LibGfx's blur functions.
    return sigma * 2;
}

Filter::DropShadow::Resolved Filter::DropShadow::resolved(Layout::Node const& node) const
{
    // The default value for omitted values is missing length values set to 0
    // and the missing used color is taken from the color property.
    return Resolved {
        offset_x.resolved(node).to_px(node).value(),
        offset_y.resolved(node).to_px(node).value(),
        radius.has_value() ? radius->resolved(node).to_px(node).value() : 0.0f,
        color.has_value() ? *color : node.computed_values().color()
    };
}

float Filter::HueRotate::angle_degrees() const
{
    // Default value when omitted is 0deg.
    if (!angle.has_value())
        return 0.0f;
    return angle->visit([&](Angle const& angle) { return angle.to_degrees(); }, [&](auto) { return 0.0f; });
}

float Filter::Color::resolved_amount() const
{
    if (amount.has_value()) {
        if (amount->is_percentage())
            return amount->percentage().as_fraction();
        return amount->number().value();
    }
    // All color filters (brightness, sepia, etc) have a default amount of 1.
    return 1.0f;
}

ErrorOr<String> FilterValueListStyleValue::to_string() const
{
    StringBuilder builder {};
    bool first = true;
    for (auto& filter_function : filter_value_list()) {
        if (!first)
            builder.append(' ');
        filter_function.visit(
            [&](Filter::Blur const& blur) {
                builder.append("blur("sv);
                if (blur.radius.has_value())
                    builder.append(blur.radius->to_string().release_value());
            },
            [&](Filter::DropShadow const& drop_shadow) {
                builder.appendff("drop-shadow({} {}"sv,
                    drop_shadow.offset_x, drop_shadow.offset_y);
                if (drop_shadow.radius.has_value())
                    builder.appendff(" {}", drop_shadow.radius->to_string());
                if (drop_shadow.color.has_value()) {
                    builder.append(' ');
                    serialize_a_srgb_value(builder, *drop_shadow.color);
                }
            },
            [&](Filter::HueRotate const& hue_rotate) {
                builder.append("hue-rotate("sv);
                if (hue_rotate.angle.has_value()) {
                    hue_rotate.angle->visit(
                        [&](Angle const& angle) {
                            builder.append(angle.to_string().release_value());
                        },
                        [&](auto&) {
                            builder.append('0');
                        });
                }
            },
            [&](Filter::Color const& color) {
                builder.appendff("{}(",
                    [&] {
                        switch (color.operation) {
                        case Filter::Color::Operation::Brightness:
                            return "brightness"sv;
                        case Filter::Color::Operation::Contrast:
                            return "contrast"sv;
                        case Filter::Color::Operation::Grayscale:
                            return "grayscale"sv;
                        case Filter::Color::Operation::Invert:
                            return "invert"sv;
                        case Filter::Color::Operation::Opacity:
                            return "opacity"sv;
                        case Filter::Color::Operation::Saturate:
                            return "saturate"sv;
                        case Filter::Color::Operation::Sepia:
                            return "sepia"sv;
                        default:
                            VERIFY_NOT_REACHED();
                        }
                    }());
                if (color.amount.has_value())
                    builder.append(color.amount->to_string().release_value());
            });
        builder.append(')');
        first = false;
    }
    return builder.to_string();
}

static bool operator==(Filter::Blur const& a, Filter::Blur const& b)
{
    return a.radius == b.radius;
}

static bool operator==(Filter::DropShadow const& a, Filter::DropShadow const& b)
{
    return a.offset_x == b.offset_x && a.offset_y == b.offset_y && a.radius == b.radius && a.color == b.color;
}

static bool operator==(Filter::HueRotate::Zero const&, Filter::HueRotate::Zero const&)
{
    return true;
}

static bool operator==(Filter::Color const& a, Filter::Color const& b)
{
    return a.operation == b.operation && a.amount == b.amount;
}

static bool operator==(Filter::HueRotate const& a, Filter::HueRotate const& b)
{
    return a.angle == b.angle;
}

static bool variant_equals(auto const& a, auto const& b)
{
    return a.visit([&](auto const& held_value) {
        using HeldType = AK::Detail::Decay<decltype(held_value)>;
        bool other_holds_same_type = b.template has<HeldType>();
        return other_holds_same_type && held_value == b.template get<HeldType>();
    });
}

static bool operator==(Filter::HueRotate::AngleOrZero const& a, Filter::HueRotate::AngleOrZero const& b)
{
    return variant_equals(a, b);
}

static bool operator==(FilterFunction const& a, FilterFunction const& b)
{
    return variant_equals(a, b);
}

bool FilterValueListStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_filter_value_list();
    if (m_filter_value_list.size() != typed_other.m_filter_value_list.size())
        return false;
    for (size_t i = 0; i < m_filter_value_list.size(); i++) {
        if (m_filter_value_list[i] != typed_other.m_filter_value_list[i])
            return false;
    }
    return true;
}

ErrorOr<String> FlexStyleValue::to_string() const
{
    return String::formatted("{} {} {}", TRY(m_grow->to_string()), TRY(m_shrink->to_string()), TRY(m_basis->to_string()));
}

bool FlexStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_flex();
    return m_grow->equals(typed_other.m_grow)
        && m_shrink->equals(typed_other.m_shrink)
        && m_basis->equals(typed_other.m_basis);
}

ErrorOr<String> FlexFlowStyleValue::to_string() const
{
    return String::formatted("{} {}", TRY(m_flex_direction->to_string()), TRY(m_flex_wrap->to_string()));
}

bool FlexFlowStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_flex_flow();
    return m_flex_direction->equals(typed_other.m_flex_direction)
        && m_flex_wrap->equals(typed_other.m_flex_wrap);
}

ErrorOr<String> FontStyleValue::to_string() const
{
    return String::formatted("{} {} {} / {} {}", TRY(m_font_style->to_string()), TRY(m_font_weight->to_string()), TRY(m_font_size->to_string()), TRY(m_line_height->to_string()), TRY(m_font_families->to_string()));
}

bool FontStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_font();
    return m_font_style->equals(typed_other.m_font_style)
        && m_font_weight->equals(typed_other.m_font_weight)
        && m_font_size->equals(typed_other.m_font_size)
        && m_line_height->equals(typed_other.m_line_height)
        && m_font_families->equals(typed_other.m_font_families);
}

bool FrequencyStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_frequency == other.as_frequency().m_frequency;
}

ErrorOr<String> GridTrackPlacementShorthandStyleValue::to_string() const
{
    if (m_end->grid_track_placement().is_auto())
        return String::formatted("{}", TRY(m_start->grid_track_placement().to_string()));
    return String::formatted("{} / {}", TRY(m_start->grid_track_placement().to_string()), TRY(m_end->grid_track_placement().to_string()));
}

bool GridTrackPlacementShorthandStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_grid_track_placement_shorthand();
    return m_start->equals(typed_other.m_start)
        && m_end->equals(typed_other.m_end);
}

ErrorOr<String> GridTrackPlacementStyleValue::to_string() const
{
    return m_grid_track_placement.to_string();
}

bool GridTrackPlacementStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_grid_track_placement();
    return m_grid_track_placement == typed_other.grid_track_placement();
}

ErrorOr<String> GridTrackSizeStyleValue::to_string() const
{
    return m_grid_track_size_list.to_string();
}

bool GridTrackSizeStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_grid_track_size_list();
    return m_grid_track_size_list == typed_other.grid_track_size_list();
}

ErrorOr<String> IdentifierStyleValue::to_string() const
{
    return String::from_utf8(CSS::string_from_value_id(m_id));
}

bool IdentifierStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_id == other.as_identifier().m_id;
}

bool IdentifierStyleValue::has_color() const
{
    switch (m_id) {
    case ValueID::Currentcolor:
    case ValueID::LibwebLink:
    case ValueID::LibwebPaletteActiveLink:
    case ValueID::LibwebPaletteActiveWindowBorder1:
    case ValueID::LibwebPaletteActiveWindowBorder2:
    case ValueID::LibwebPaletteActiveWindowTitle:
    case ValueID::LibwebPaletteBase:
    case ValueID::LibwebPaletteBaseText:
    case ValueID::LibwebPaletteButton:
    case ValueID::LibwebPaletteButtonText:
    case ValueID::LibwebPaletteDesktopBackground:
    case ValueID::LibwebPaletteFocusOutline:
    case ValueID::LibwebPaletteHighlightWindowBorder1:
    case ValueID::LibwebPaletteHighlightWindowBorder2:
    case ValueID::LibwebPaletteHighlightWindowTitle:
    case ValueID::LibwebPaletteHoverHighlight:
    case ValueID::LibwebPaletteInactiveSelection:
    case ValueID::LibwebPaletteInactiveSelectionText:
    case ValueID::LibwebPaletteInactiveWindowBorder1:
    case ValueID::LibwebPaletteInactiveWindowBorder2:
    case ValueID::LibwebPaletteInactiveWindowTitle:
    case ValueID::LibwebPaletteLink:
    case ValueID::LibwebPaletteMenuBase:
    case ValueID::LibwebPaletteMenuBaseText:
    case ValueID::LibwebPaletteMenuSelection:
    case ValueID::LibwebPaletteMenuSelectionText:
    case ValueID::LibwebPaletteMenuStripe:
    case ValueID::LibwebPaletteMovingWindowBorder1:
    case ValueID::LibwebPaletteMovingWindowBorder2:
    case ValueID::LibwebPaletteMovingWindowTitle:
    case ValueID::LibwebPaletteRubberBandBorder:
    case ValueID::LibwebPaletteRubberBandFill:
    case ValueID::LibwebPaletteRuler:
    case ValueID::LibwebPaletteRulerActiveText:
    case ValueID::LibwebPaletteRulerBorder:
    case ValueID::LibwebPaletteRulerInactiveText:
    case ValueID::LibwebPaletteSelection:
    case ValueID::LibwebPaletteSelectionText:
    case ValueID::LibwebPaletteSyntaxComment:
    case ValueID::LibwebPaletteSyntaxControlKeyword:
    case ValueID::LibwebPaletteSyntaxIdentifier:
    case ValueID::LibwebPaletteSyntaxKeyword:
    case ValueID::LibwebPaletteSyntaxNumber:
    case ValueID::LibwebPaletteSyntaxOperator:
    case ValueID::LibwebPaletteSyntaxPreprocessorStatement:
    case ValueID::LibwebPaletteSyntaxPreprocessorValue:
    case ValueID::LibwebPaletteSyntaxPunctuation:
    case ValueID::LibwebPaletteSyntaxString:
    case ValueID::LibwebPaletteSyntaxType:
    case ValueID::LibwebPaletteTextCursor:
    case ValueID::LibwebPaletteThreedHighlight:
    case ValueID::LibwebPaletteThreedShadow1:
    case ValueID::LibwebPaletteThreedShadow2:
    case ValueID::LibwebPaletteVisitedLink:
    case ValueID::LibwebPaletteWindow:
    case ValueID::LibwebPaletteWindowText:
        return true;
    default:
        return false;
    }
}

Color IdentifierStyleValue::to_color(Layout::NodeWithStyle const& node) const
{
    if (id() == CSS::ValueID::Currentcolor) {
        if (!node.has_style())
            return Color::Black;
        return node.computed_values().color();
    }

    auto& document = node.document();
    if (id() == CSS::ValueID::LibwebLink)
        return document.link_color();

    if (!document.page())
        return {};

    auto palette = document.page()->palette();
    switch (id()) {
    case CSS::ValueID::LibwebPaletteDesktopBackground:
        return palette.color(ColorRole::DesktopBackground);
    case CSS::ValueID::LibwebPaletteActiveWindowBorder1:
        return palette.color(ColorRole::ActiveWindowBorder1);
    case CSS::ValueID::LibwebPaletteActiveWindowBorder2:
        return palette.color(ColorRole::ActiveWindowBorder2);
    case CSS::ValueID::LibwebPaletteActiveWindowTitle:
        return palette.color(ColorRole::ActiveWindowTitle);
    case CSS::ValueID::LibwebPaletteInactiveWindowBorder1:
        return palette.color(ColorRole::InactiveWindowBorder1);
    case CSS::ValueID::LibwebPaletteInactiveWindowBorder2:
        return palette.color(ColorRole::InactiveWindowBorder2);
    case CSS::ValueID::LibwebPaletteInactiveWindowTitle:
        return palette.color(ColorRole::InactiveWindowTitle);
    case CSS::ValueID::LibwebPaletteMovingWindowBorder1:
        return palette.color(ColorRole::MovingWindowBorder1);
    case CSS::ValueID::LibwebPaletteMovingWindowBorder2:
        return palette.color(ColorRole::MovingWindowBorder2);
    case CSS::ValueID::LibwebPaletteMovingWindowTitle:
        return palette.color(ColorRole::MovingWindowTitle);
    case CSS::ValueID::LibwebPaletteHighlightWindowBorder1:
        return palette.color(ColorRole::HighlightWindowBorder1);
    case CSS::ValueID::LibwebPaletteHighlightWindowBorder2:
        return palette.color(ColorRole::HighlightWindowBorder2);
    case CSS::ValueID::LibwebPaletteHighlightWindowTitle:
        return palette.color(ColorRole::HighlightWindowTitle);
    case CSS::ValueID::LibwebPaletteMenuStripe:
        return palette.color(ColorRole::MenuStripe);
    case CSS::ValueID::LibwebPaletteMenuBase:
        return palette.color(ColorRole::MenuBase);
    case CSS::ValueID::LibwebPaletteMenuBaseText:
        return palette.color(ColorRole::MenuBaseText);
    case CSS::ValueID::LibwebPaletteMenuSelection:
        return palette.color(ColorRole::MenuSelection);
    case CSS::ValueID::LibwebPaletteMenuSelectionText:
        return palette.color(ColorRole::MenuSelectionText);
    case CSS::ValueID::LibwebPaletteWindow:
        return palette.color(ColorRole::Window);
    case CSS::ValueID::LibwebPaletteWindowText:
        return palette.color(ColorRole::WindowText);
    case CSS::ValueID::LibwebPaletteButton:
        return palette.color(ColorRole::Button);
    case CSS::ValueID::LibwebPaletteButtonText:
        return palette.color(ColorRole::ButtonText);
    case CSS::ValueID::LibwebPaletteBase:
        return palette.color(ColorRole::Base);
    case CSS::ValueID::LibwebPaletteBaseText:
        return palette.color(ColorRole::BaseText);
    case CSS::ValueID::LibwebPaletteThreedHighlight:
        return palette.color(ColorRole::ThreedHighlight);
    case CSS::ValueID::LibwebPaletteThreedShadow1:
        return palette.color(ColorRole::ThreedShadow1);
    case CSS::ValueID::LibwebPaletteThreedShadow2:
        return palette.color(ColorRole::ThreedShadow2);
    case CSS::ValueID::LibwebPaletteHoverHighlight:
        return palette.color(ColorRole::HoverHighlight);
    case CSS::ValueID::LibwebPaletteSelection:
        return palette.color(ColorRole::Selection);
    case CSS::ValueID::LibwebPaletteSelectionText:
        return palette.color(ColorRole::SelectionText);
    case CSS::ValueID::LibwebPaletteInactiveSelection:
        return palette.color(ColorRole::InactiveSelection);
    case CSS::ValueID::LibwebPaletteInactiveSelectionText:
        return palette.color(ColorRole::InactiveSelectionText);
    case CSS::ValueID::LibwebPaletteRubberBandFill:
        return palette.color(ColorRole::RubberBandFill);
    case CSS::ValueID::LibwebPaletteRubberBandBorder:
        return palette.color(ColorRole::RubberBandBorder);
    case CSS::ValueID::LibwebPaletteLink:
        return palette.color(ColorRole::Link);
    case CSS::ValueID::LibwebPaletteActiveLink:
        return palette.color(ColorRole::ActiveLink);
    case CSS::ValueID::LibwebPaletteVisitedLink:
        return palette.color(ColorRole::VisitedLink);
    case CSS::ValueID::LibwebPaletteRuler:
        return palette.color(ColorRole::Ruler);
    case CSS::ValueID::LibwebPaletteRulerBorder:
        return palette.color(ColorRole::RulerBorder);
    case CSS::ValueID::LibwebPaletteRulerActiveText:
        return palette.color(ColorRole::RulerActiveText);
    case CSS::ValueID::LibwebPaletteRulerInactiveText:
        return palette.color(ColorRole::RulerInactiveText);
    case CSS::ValueID::LibwebPaletteTextCursor:
        return palette.color(ColorRole::TextCursor);
    case CSS::ValueID::LibwebPaletteFocusOutline:
        return palette.color(ColorRole::FocusOutline);
    case CSS::ValueID::LibwebPaletteSyntaxComment:
        return palette.color(ColorRole::SyntaxComment);
    case CSS::ValueID::LibwebPaletteSyntaxNumber:
        return palette.color(ColorRole::SyntaxNumber);
    case CSS::ValueID::LibwebPaletteSyntaxString:
        return palette.color(ColorRole::SyntaxString);
    case CSS::ValueID::LibwebPaletteSyntaxType:
        return palette.color(ColorRole::SyntaxType);
    case CSS::ValueID::LibwebPaletteSyntaxPunctuation:
        return palette.color(ColorRole::SyntaxPunctuation);
    case CSS::ValueID::LibwebPaletteSyntaxOperator:
        return palette.color(ColorRole::SyntaxOperator);
    case CSS::ValueID::LibwebPaletteSyntaxKeyword:
        return palette.color(ColorRole::SyntaxKeyword);
    case CSS::ValueID::LibwebPaletteSyntaxControlKeyword:
        return palette.color(ColorRole::SyntaxControlKeyword);
    case CSS::ValueID::LibwebPaletteSyntaxIdentifier:
        return palette.color(ColorRole::SyntaxIdentifier);
    case CSS::ValueID::LibwebPaletteSyntaxPreprocessorStatement:
        return palette.color(ColorRole::SyntaxPreprocessorStatement);
    case CSS::ValueID::LibwebPaletteSyntaxPreprocessorValue:
        return palette.color(ColorRole::SyntaxPreprocessorValue);
    default:
        return {};
    }
}

ImageStyleValue::ImageStyleValue(AK::URL const& url)
    : AbstractImageStyleValue(Type::Image)
    , m_url(url)
{
}

void ImageStyleValue::load_any_resources(DOM::Document& document)
{
    if (resource())
        return;

    m_document = &document;
    auto request = LoadRequest::create_for_url_on_page(m_url, document.page());
    set_resource(ResourceLoader::the().load_resource(Resource::Type::Image, request));
}

void ImageStyleValue::resource_did_load()
{
    if (!m_document)
        return;
    // FIXME: Do less than a full repaint if possible?
    if (m_document && m_document->browsing_context())
        m_document->browsing_context()->set_needs_display();

    if (resource()->is_animated() && resource()->frame_count() > 1) {
        m_timer = Platform::Timer::create();
        m_timer->set_interval(resource()->frame_duration(0));
        m_timer->on_timeout = [this] { animate(); };
        m_timer->start();
    }
}

void ImageStyleValue::animate()
{
    m_current_frame_index = (m_current_frame_index + 1) % resource()->frame_count();
    auto current_frame_duration = resource()->frame_duration(m_current_frame_index);

    if (current_frame_duration != m_timer->interval())
        m_timer->restart(current_frame_duration);

    if (m_current_frame_index == resource()->frame_count() - 1) {
        ++m_loops_completed;
        if (m_loops_completed > 0 && m_loops_completed == resource()->loop_count())
            m_timer->stop();
    }

    if (on_animate)
        on_animate();
}

Gfx::Bitmap const* ImageStyleValue::bitmap(size_t frame_index) const
{
    if (!resource())
        return nullptr;
    return resource()->bitmap(frame_index);
}

ErrorOr<String> ImageStyleValue::to_string() const
{
    return String::from_deprecated_string(serialize_a_url(m_url.to_deprecated_string()));
}

bool ImageStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_url == other.as_image().m_url;
}

Optional<CSSPixels> ImageStyleValue::natural_width() const
{
    if (auto* b = bitmap(0); b != nullptr)
        return b->width();
    return {};
}

Optional<CSSPixels> ImageStyleValue::natural_height() const
{
    if (auto* b = bitmap(0); b != nullptr)
        return b->height();
    return {};
}

void ImageStyleValue::paint(PaintContext& context, DevicePixelRect const& dest_rect, CSS::ImageRendering image_rendering) const
{
    if (auto* b = bitmap(m_current_frame_index); b != nullptr)
        context.painter().draw_scaled_bitmap(dest_rect.to_type<int>(), *b, bitmap(0)->rect(), 1.0f, to_gfx_scaling_mode(image_rendering));
}

static void serialize_color_stop_list(StringBuilder& builder, auto const& color_stop_list)
{
    bool first = true;
    for (auto const& element : color_stop_list) {
        if (!first)
            builder.append(", "sv);

        if (element.transition_hint.has_value()) {
            builder.appendff("{}, "sv, element.transition_hint->value.to_string());
        }

        serialize_a_srgb_value(builder, element.color_stop.color);
        for (auto position : Array { &element.color_stop.position, &element.color_stop.second_position }) {
            if (position->has_value())
                builder.appendff(" {}"sv, (*position)->to_string());
        }
        first = false;
    }
}

ErrorOr<String> LinearGradientStyleValue::to_string() const
{
    StringBuilder builder;
    auto side_or_corner_to_string = [](SideOrCorner value) {
        switch (value) {
        case SideOrCorner::Top:
            return "top"sv;
        case SideOrCorner::Bottom:
            return "bottom"sv;
        case SideOrCorner::Left:
            return "left"sv;
        case SideOrCorner::Right:
            return "right"sv;
        case SideOrCorner::TopLeft:
            return "top left"sv;
        case SideOrCorner::TopRight:
            return "top right"sv;
        case SideOrCorner::BottomLeft:
            return "bottom left"sv;
        case SideOrCorner::BottomRight:
            return "bottom right"sv;
        default:
            VERIFY_NOT_REACHED();
        }
    };

    if (m_gradient_type == GradientType::WebKit)
        builder.append("-webkit-"sv);
    if (is_repeating())
        builder.append("repeating-"sv);
    builder.append("linear-gradient("sv);
    m_direction.visit(
        [&](SideOrCorner side_or_corner) {
            builder.appendff("{}{}, "sv, m_gradient_type == GradientType::Standard ? "to "sv : ""sv, side_or_corner_to_string(side_or_corner));
        },
        [&](Angle const& angle) {
            builder.appendff("{}, "sv, angle.to_string());
        });

    serialize_color_stop_list(builder, m_color_stop_list);
    builder.append(")"sv);
    return builder.to_string();
}

static bool operator==(LinearGradientStyleValue::GradientDirection const& a, LinearGradientStyleValue::GradientDirection const& b)
{
    if (a.has<SideOrCorner>() && b.has<SideOrCorner>())
        return a.get<SideOrCorner>() == b.get<SideOrCorner>();
    if (a.has<Angle>() && b.has<Angle>())
        return a.get<Angle>() == b.get<Angle>();
    return false;
}

static bool operator==(EdgeRect const& a, EdgeRect const& b)
{
    return a.top_edge == b.top_edge && a.right_edge == b.right_edge && a.bottom_edge == b.bottom_edge && a.left_edge == b.left_edge;
}

bool LinearGradientStyleValue::equals(StyleValue const& other_) const
{
    if (type() != other_.type())
        return false;
    auto& other = other_.as_linear_gradient();
    return (m_gradient_type == other.m_gradient_type
        && m_repeating == other.m_repeating
        && m_direction == other.m_direction
        && m_color_stop_list == other.m_color_stop_list);
}

float LinearGradientStyleValue::angle_degrees(CSSPixelSize gradient_size) const
{
    auto corner_angle_degrees = [&] {
        return static_cast<float>(atan2(gradient_size.height().value(), gradient_size.width().value())) * 180 / AK::Pi<float>;
    };
    return m_direction.visit(
        [&](SideOrCorner side_or_corner) {
            auto angle = [&] {
                switch (side_or_corner) {
                case SideOrCorner::Top:
                    return 0.0f;
                case SideOrCorner::Bottom:
                    return 180.0f;
                case SideOrCorner::Left:
                    return 270.0f;
                case SideOrCorner::Right:
                    return 90.0f;
                case SideOrCorner::TopRight:
                    return corner_angle_degrees();
                case SideOrCorner::BottomLeft:
                    return corner_angle_degrees() + 180.0f;
                case SideOrCorner::TopLeft:
                    return -corner_angle_degrees();
                case SideOrCorner::BottomRight:
                    return -(corner_angle_degrees() + 180.0f);
                default:
                    VERIFY_NOT_REACHED();
                }
            }();
            // Note: For unknowable reasons the angles are opposite on the -webkit- version
            if (m_gradient_type == GradientType::WebKit)
                return angle + 180.0f;
            return angle;
        },
        [&](Angle const& angle) {
            return angle.to_degrees();
        });
}

void LinearGradientStyleValue::resolve_for_size(Layout::Node const& node, CSSPixelSize size) const
{
    if (m_resolved.has_value() && m_resolved->size == size)
        return;
    m_resolved = ResolvedData { Painting::resolve_linear_gradient_data(node, size, *this), size };
}

void LinearGradientStyleValue::paint(PaintContext& context, DevicePixelRect const& dest_rect, CSS::ImageRendering) const
{
    VERIFY(m_resolved.has_value());
    Painting::paint_linear_gradient(context, dest_rect, m_resolved->data);
}

CSSPixelPoint PositionValue::resolved(Layout::Node const& node, CSSPixelRect const& rect) const
{
    // Note: A preset + a none default x/y_relative_to is impossible in the syntax (and makes little sense)
    CSSPixels x = horizontal_position.visit(
        [&](HorizontalPreset preset) -> CSSPixels {
            return rect.width() * [&] {
                switch (preset) {
                case HorizontalPreset::Left:
                    return 0.0f;
                case HorizontalPreset::Center:
                    return 0.5f;
                case HorizontalPreset::Right:
                    return 1.0f;
                default:
                    VERIFY_NOT_REACHED();
                }
            }();
        },
        [&](LengthPercentage length_percentage) -> CSSPixels {
            return length_percentage.resolved(node, Length::make_px(rect.width())).to_px(node);
        });
    CSSPixels y = vertical_position.visit(
        [&](VerticalPreset preset) -> CSSPixels {
            return rect.height() * [&] {
                switch (preset) {
                case VerticalPreset::Top:
                    return 0.0f;
                case VerticalPreset::Center:
                    return 0.5f;
                case VerticalPreset::Bottom:
                    return 1.0f;
                default:
                    VERIFY_NOT_REACHED();
                }
            }();
        },
        [&](LengthPercentage length_percentage) -> CSSPixels {
            return length_percentage.resolved(node, Length::make_px(rect.height())).to_px(node);
        });
    if (x_relative_to == HorizontalEdge::Right)
        x = rect.width() - x;
    if (y_relative_to == VerticalEdge::Bottom)
        y = rect.height() - y;
    return CSSPixelPoint { rect.x() + x, rect.y() + y };
}

void PositionValue::serialize(StringBuilder& builder) const
{
    // Note: This means our serialization with simplify any with explicit edges that are just `top left`.
    bool has_relative_edges = x_relative_to == HorizontalEdge::Right || y_relative_to == VerticalEdge::Bottom;
    if (has_relative_edges)
        builder.append(x_relative_to == HorizontalEdge::Left ? "left "sv : "right "sv);
    horizontal_position.visit(
        [&](HorizontalPreset preset) {
            builder.append([&] {
                switch (preset) {
                case HorizontalPreset::Left:
                    return "left"sv;
                case HorizontalPreset::Center:
                    return "center"sv;
                case HorizontalPreset::Right:
                    return "right"sv;
                default:
                    VERIFY_NOT_REACHED();
                }
            }());
        },
        [&](LengthPercentage length_percentage) {
            builder.append(length_percentage.to_string().release_value_but_fixme_should_propagate_errors());
        });
    builder.append(' ');
    if (has_relative_edges)
        builder.append(y_relative_to == VerticalEdge::Top ? "top "sv : "bottom "sv);
    vertical_position.visit(
        [&](VerticalPreset preset) {
            builder.append([&] {
                switch (preset) {
                case VerticalPreset::Top:
                    return "top"sv;
                case VerticalPreset::Center:
                    return "center"sv;
                case VerticalPreset::Bottom:
                    return "bottom"sv;
                default:
                    VERIFY_NOT_REACHED();
                }
            }());
        },
        [&](LengthPercentage length_percentage) {
            builder.append(length_percentage.to_string().release_value_but_fixme_should_propagate_errors());
        });
}

bool PositionValue::operator==(PositionValue const& other) const
{
    return (
        x_relative_to == other.x_relative_to
        && y_relative_to == other.y_relative_to
        && variant_equals(horizontal_position, other.horizontal_position)
        && variant_equals(vertical_position, other.vertical_position));
}

ErrorOr<String> RadialGradientStyleValue::to_string() const
{
    StringBuilder builder;
    if (is_repeating())
        builder.append("repeating-"sv);
    builder.appendff("radial-gradient({} "sv,
        m_ending_shape == EndingShape::Circle ? "circle"sv : "ellipse"sv);

    m_size.visit(
        [&](Extent extent) {
            builder.append([&] {
                switch (extent) {
                case Extent::ClosestCorner:
                    return "closest-corner"sv;
                case Extent::ClosestSide:
                    return "closest-side"sv;
                case Extent::FarthestCorner:
                    return "farthest-corner"sv;
                case Extent::FarthestSide:
                    return "farthest-side"sv;
                default:
                    VERIFY_NOT_REACHED();
                }
            }());
        },
        [&](CircleSize const& circle_size) { builder.append(circle_size.radius.to_string().release_value()); },
        [&](EllipseSize const& ellipse_size) { builder.appendff("{} {}", ellipse_size.radius_a.to_string(), ellipse_size.radius_b.to_string()); });

    if (m_position != PositionValue::center()) {
        builder.appendff(" at "sv);
        m_position.serialize(builder);
    }

    builder.append(", "sv);
    serialize_color_stop_list(builder, m_color_stop_list);
    builder.append(')');
    return builder.to_string();
}

Gfx::FloatSize RadialGradientStyleValue::resolve_size(Layout::Node const& node, Gfx::FloatPoint center, Gfx::FloatRect const& size) const
{
    auto const side_shape = [&](auto distance_function) {
        auto const distance_from = [&](float v, float a, float b, auto distance_function) {
            return distance_function(fabs(a - v), fabs(b - v));
        };
        auto x_dist = distance_from(center.x(), size.left(), size.right(), distance_function);
        auto y_dist = distance_from(center.y(), size.top(), size.bottom(), distance_function);
        if (m_ending_shape == EndingShape::Circle) {
            auto dist = distance_function(x_dist, y_dist);
            return Gfx::FloatSize { dist, dist };
        } else {
            return Gfx::FloatSize { x_dist, y_dist };
        }
    };

    auto const closest_side_shape = [&] {
        return side_shape(AK::min<float>);
    };

    auto const farthest_side_shape = [&] {
        return side_shape(AK::max<float>);
    };

    auto const corner_distance = [&](auto distance_compare, Gfx::FloatPoint& corner) {
        auto top_left_distance = size.top_left().distance_from(center);
        auto top_right_distance = size.top_right().distance_from(center);
        auto bottom_right_distance = size.bottom_right().distance_from(center);
        auto bottom_left_distance = size.bottom_left().distance_from(center);
        auto distance = top_left_distance;
        if (distance_compare(top_right_distance, distance)) {
            corner = size.top_right();
            distance = top_right_distance;
        }
        if (distance_compare(bottom_right_distance, distance)) {
            corner = size.top_right();
            distance = bottom_right_distance;
        }
        if (distance_compare(bottom_left_distance, distance)) {
            corner = size.top_right();
            distance = bottom_left_distance;
        }
        return distance;
    };

    auto const closest_corner_distance = [&](Gfx::FloatPoint& corner) {
        return corner_distance([](float a, float b) { return a < b; }, corner);
    };

    auto const farthest_corner_distance = [&](Gfx::FloatPoint& corner) {
        return corner_distance([](float a, float b) { return a > b; }, corner);
    };

    auto const corner_shape = [&](auto corner_distance, auto get_shape) {
        Gfx::FloatPoint corner {};
        auto distance = corner_distance(corner);
        if (m_ending_shape == EndingShape::Ellipse) {
            auto shape = get_shape();
            auto aspect_ratio = shape.width() / shape.height();
            auto p = corner - center;
            auto radius_a = AK::sqrt(p.y() * p.y() * aspect_ratio * aspect_ratio + p.x() * p.x());
            auto radius_b = radius_a / aspect_ratio;
            return Gfx::FloatSize { radius_a, radius_b };
        }
        return Gfx::FloatSize { distance, distance };
    };

    // https://w3c.github.io/csswg-drafts/css-images/#radial-gradient-syntax
    auto resolved_size = m_size.visit(
        [&](Extent extent) {
            switch (extent) {
            case Extent::ClosestSide:
                // The ending shape is sized so that it exactly meets the side of the gradient box closest to the gradient’s center.
                // If the shape is an ellipse, it exactly meets the closest side in each dimension.
                return closest_side_shape();
            case Extent::ClosestCorner:
                // The ending shape is sized so that it passes through the corner of the gradient box closest to the gradient’s center.
                // If the shape is an ellipse, the ending shape is given the same aspect-ratio it would have if closest-side were specified
                return corner_shape(closest_corner_distance, closest_side_shape);
            case Extent::FarthestCorner:
                // Same as closest-corner, except the ending shape is sized based on the farthest corner.
                // If the shape is an ellipse, the ending shape is given the same aspect ratio it would have if farthest-side were specified.
                return corner_shape(farthest_corner_distance, farthest_side_shape);
            case Extent::FarthestSide:
                // Same as closest-side, except the ending shape is sized based on the farthest side(s).
                return farthest_side_shape();
            default:
                VERIFY_NOT_REACHED();
            }
        },
        [&](CircleSize const& circle_size) {
            auto radius = circle_size.radius.to_px(node);
            return Gfx::FloatSize { radius, radius };
        },
        [&](EllipseSize const& ellipse_size) {
            auto radius_a = ellipse_size.radius_a.resolved(node, CSS::Length::make_px(size.width())).to_px(node);
            auto radius_b = ellipse_size.radius_b.resolved(node, CSS::Length::make_px(size.height())).to_px(node);
            return Gfx::FloatSize { radius_a, radius_b };
        });

    // Handle degenerate cases
    // https://w3c.github.io/csswg-drafts/css-images/#degenerate-radials

    constexpr auto arbitrary_small_number = 1e-10;
    constexpr auto arbitrary_large_number = 1e10;

    // If the ending shape is a circle with zero radius:
    if (m_ending_shape == EndingShape::Circle && resolved_size.is_empty()) {
        // Render as if the ending shape was a circle whose radius was an arbitrary very small number greater than zero.
        // This will make the gradient continue to look like a circle.
        return Gfx::FloatSize { arbitrary_small_number, arbitrary_small_number };
    }
    // If the ending shape has zero width (regardless of the height):
    if (resolved_size.width() <= 0) {
        // Render as if the ending shape was an ellipse whose height was an arbitrary very large number
        // and whose width was an arbitrary very small number greater than zero.
        // This will make the gradient look similar to a horizontal linear gradient that is mirrored across the center of the ellipse.
        // It also means that all color-stop positions specified with a percentage resolve to 0px.
        return Gfx::FloatSize { arbitrary_small_number, arbitrary_large_number };
    }
    // Otherwise, if the ending shape has zero height:
    if (resolved_size.height() <= 0) {
        // Render as if the ending shape was an ellipse whose width was an arbitrary very large number and whose height
        // was an arbitrary very small number greater than zero. This will make the gradient look like a solid-color image equal
        // to the color of the last color-stop, or equal to the average color of the gradient if it’s repeating.
        return Gfx::FloatSize { arbitrary_large_number, arbitrary_small_number };
    }
    return resolved_size;
}

void RadialGradientStyleValue::resolve_for_size(Layout::Node const& node, CSSPixelSize paint_size) const
{
    CSSPixelRect gradient_box { { 0, 0 }, paint_size };
    auto center = m_position.resolved(node, gradient_box).to_type<float>();
    auto gradient_size = resolve_size(node, center, gradient_box.to_type<float>());
    if (m_resolved.has_value() && m_resolved->gradient_size == gradient_size)
        return;
    m_resolved = ResolvedData {
        Painting::resolve_radial_gradient_data(node, gradient_size.to_type<CSSPixels>(), *this),
        gradient_size,
        center,
    };
}

bool RadialGradientStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto& other_gradient = other.as_radial_gradient();
    return (m_ending_shape == other_gradient.m_ending_shape
        && variant_equals(m_size, other_gradient.m_size)
        && m_position == other_gradient.m_position
        && m_color_stop_list == other_gradient.m_color_stop_list);
}

void RadialGradientStyleValue::paint(PaintContext& context, DevicePixelRect const& dest_rect, CSS::ImageRendering) const
{
    VERIFY(m_resolved.has_value());
    Painting::paint_radial_gradient(context, dest_rect, m_resolved->data,
        context.rounded_device_point(m_resolved->center.to_type<CSSPixels>()),
        context.rounded_device_size(m_resolved->gradient_size.to_type<CSSPixels>()));
}

ErrorOr<String> ConicGradientStyleValue::to_string() const
{
    StringBuilder builder;
    if (is_repeating())
        builder.append("repeating-"sv);
    builder.append("conic-gradient("sv);
    bool has_from_angle = false;
    bool has_at_position = false;
    if ((has_from_angle = m_from_angle.to_degrees() != 0))
        builder.appendff("from {}", TRY(m_from_angle.to_string()));
    if ((has_at_position = m_position != PositionValue::center())) {
        if (has_from_angle)
            builder.append(' ');
        builder.appendff("at "sv);
        m_position.serialize(builder);
    }
    if (has_from_angle || has_at_position)
        builder.append(", "sv);
    serialize_color_stop_list(builder, m_color_stop_list);
    builder.append(')');
    return builder.to_string();
}

void ConicGradientStyleValue::resolve_for_size(Layout::Node const& node, CSSPixelSize size) const
{
    if (!m_resolved.has_value())
        m_resolved = ResolvedData { Painting::resolve_conic_gradient_data(node, *this), {} };
    m_resolved->position = m_position.resolved(node, CSSPixelRect { { 0, 0 }, size });
}

void ConicGradientStyleValue::paint(PaintContext& context, DevicePixelRect const& dest_rect, CSS::ImageRendering) const
{
    VERIFY(m_resolved.has_value());
    Painting::paint_conic_gradient(context, dest_rect, m_resolved->data, context.rounded_device_point(m_resolved->position));
}

bool ConicGradientStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto& other_gradient = other.as_conic_gradient();
    return (m_from_angle == other_gradient.m_from_angle
        && m_position == other_gradient.m_position
        && m_color_stop_list == other_gradient.m_color_stop_list
        && m_repeating == other_gradient.m_repeating);
}

float ConicGradientStyleValue::angle_degrees() const
{
    return m_from_angle.to_degrees();
}

bool InheritStyleValue::equals(StyleValue const& other) const
{
    return type() == other.type();
}

bool InitialStyleValue::equals(StyleValue const& other) const
{
    return type() == other.type();
}

bool LengthStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_length == other.as_length().m_length;
}

ErrorOr<String> ListStyleStyleValue::to_string() const
{
    return String::formatted("{} {} {}", TRY(m_position->to_string()), TRY(m_image->to_string()), TRY(m_style_type->to_string()));
}

bool ListStyleStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_list_style();
    return m_position->equals(typed_other.m_position)
        && m_image->equals(typed_other.m_image)
        && m_style_type->equals(typed_other.m_style_type);
}

ErrorOr<String> NumericStyleValue::to_string() const
{
    return m_value.visit(
        [](float value) {
            return String::formatted("{}", value);
        },
        [](i64 value) {
            return String::formatted("{}", value);
        });
}

bool NumericStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    if (has_integer() != other.has_integer())
        return false;
    if (has_integer())
        return m_value.get<i64>() == other.as_numeric().m_value.get<i64>();
    return m_value.get<float>() == other.as_numeric().m_value.get<float>();
}

ErrorOr<String> OverflowStyleValue::to_string() const
{
    return String::formatted("{} {}", TRY(m_overflow_x->to_string()), TRY(m_overflow_y->to_string()));
}

bool OverflowStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_overflow();
    return m_overflow_x->equals(typed_other.m_overflow_x)
        && m_overflow_y->equals(typed_other.m_overflow_y);
}

ErrorOr<String> PercentageStyleValue::to_string() const
{
    return m_percentage.to_string();
}

bool PercentageStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_percentage == other.as_percentage().m_percentage;
}

ErrorOr<String> PositionStyleValue::to_string() const
{
    auto to_string = [](PositionEdge edge) {
        switch (edge) {
        case PositionEdge::Left:
            return "left";
        case PositionEdge::Right:
            return "right";
        case PositionEdge::Top:
            return "top";
        case PositionEdge::Bottom:
            return "bottom";
        }
        VERIFY_NOT_REACHED();
    };

    return String::formatted("{} {} {} {}", to_string(m_edge_x), m_offset_x.to_string(), to_string(m_edge_y), TRY(m_offset_y.to_string()));
}

bool PositionStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_position();
    return m_edge_x == typed_other.m_edge_x
        && m_offset_x == typed_other.m_offset_x
        && m_edge_y == typed_other.m_edge_y
        && m_offset_y == typed_other.m_offset_y;
}

ErrorOr<String> RectStyleValue::to_string() const
{
    return String::formatted("rect({} {} {} {})", m_rect.top_edge, m_rect.right_edge, m_rect.bottom_edge, m_rect.left_edge);
}

bool RectStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_rect();
    return m_rect == typed_other.rect();
}

bool ResolutionStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_resolution == other.as_resolution().m_resolution;
}

ErrorOr<String> ShadowStyleValue::to_string() const
{
    StringBuilder builder;
    builder.appendff("{} {} {} {} {}", String::from_deprecated_string(m_color.to_deprecated_string()), TRY(m_offset_x.to_string()), TRY(m_offset_y.to_string()), TRY(m_blur_radius.to_string()), TRY(m_spread_distance.to_string()));
    if (m_placement == ShadowPlacement::Inner)
        builder.append(" inset"sv);
    return builder.to_string();
}

bool ShadowStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_shadow();
    return m_color == typed_other.m_color
        && m_offset_x == typed_other.m_offset_x
        && m_offset_y == typed_other.m_offset_y
        && m_blur_radius == typed_other.m_blur_radius
        && m_spread_distance == typed_other.m_spread_distance
        && m_placement == typed_other.m_placement;
}

bool StringStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_string == other.as_string().m_string;
}

ErrorOr<String> TextDecorationStyleValue::to_string() const
{
    return String::formatted("{} {} {} {}", TRY(m_line->to_string()), TRY(m_thickness->to_string()), TRY(m_style->to_string()), TRY(m_color->to_string()));
}

bool TextDecorationStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_text_decoration();
    return m_line->equals(typed_other.m_line)
        && m_thickness->equals(typed_other.m_thickness)
        && m_style->equals(typed_other.m_style)
        && m_color->equals(typed_other.m_color);
}

bool TimeStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    return m_time == other.as_time().m_time;
}

ErrorOr<String> TransformationStyleValue::to_string() const
{
    StringBuilder builder;
    builder.append(CSS::to_string(m_transform_function));
    builder.append('(');
    builder.join(", "sv, m_values);
    builder.append(')');

    return builder.to_string();
}

bool TransformationStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_transformation();
    if (m_transform_function != typed_other.m_transform_function)
        return false;
    if (m_values.size() != typed_other.m_values.size())
        return false;
    for (size_t i = 0; i < m_values.size(); ++i) {
        if (!m_values[i].equals(typed_other.m_values[i]))
            return false;
    }
    return true;
}

ErrorOr<String> UnresolvedStyleValue::to_string() const
{
    StringBuilder builder;
    for (auto& value : m_values)
        builder.append(String::from_deprecated_string(value.to_deprecated_string()).release_value());
    return builder.to_string();
}

bool UnresolvedStyleValue::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    // This is a case where comparing the strings actually makes sense.
    return to_string().release_value_but_fixme_should_propagate_errors() == other.to_string().release_value_but_fixme_should_propagate_errors();
}

bool UnsetStyleValue::equals(StyleValue const& other) const
{
    return type() == other.type();
}

ErrorOr<String> StyleValueList::to_string() const
{
    auto separator = ""sv;
    switch (m_separator) {
    case Separator::Space:
        separator = " "sv;
        break;
    case Separator::Comma:
        separator = ", "sv;
        break;
    default:
        VERIFY_NOT_REACHED();
    }

    return String::from_deprecated_string(DeprecatedString::join(separator, m_values));
}

bool StyleValueList::equals(StyleValue const& other) const
{
    if (type() != other.type())
        return false;
    auto const& typed_other = other.as_value_list();
    if (m_separator != typed_other.m_separator)
        return false;
    if (m_values.size() != typed_other.m_values.size())
        return false;
    for (size_t i = 0; i < m_values.size(); ++i) {
        if (!m_values[i].equals(typed_other.m_values[i]))
            return false;
    }
    return true;
}

NonnullRefPtr<ColorStyleValue> ColorStyleValue::create(Color color)
{
    if (color.value() == 0) {
        static auto transparent = adopt_ref(*new ColorStyleValue(color));
        return transparent;
    }

    if (color == Color::from_rgb(0x000000)) {
        static auto black = adopt_ref(*new ColorStyleValue(color));
        return black;
    }

    if (color == Color::from_rgb(0xffffff)) {
        static auto white = adopt_ref(*new ColorStyleValue(color));
        return white;
    }

    return adopt_ref(*new ColorStyleValue(color));
}

NonnullRefPtr<GridTrackPlacementStyleValue> GridTrackPlacementStyleValue::create(CSS::GridTrackPlacement grid_track_placement)
{
    return adopt_ref(*new GridTrackPlacementStyleValue(grid_track_placement));
}

NonnullRefPtr<GridTrackSizeStyleValue> GridTrackSizeStyleValue::create(CSS::GridTrackSizeList grid_track_size_list)
{
    return adopt_ref(*new GridTrackSizeStyleValue(grid_track_size_list));
}

NonnullRefPtr<GridTrackSizeStyleValue> GridTrackSizeStyleValue::make_auto()
{
    return adopt_ref(*new GridTrackSizeStyleValue(CSS::GridTrackSizeList()));
}

NonnullRefPtr<RectStyleValue> RectStyleValue::create(EdgeRect rect)
{
    return adopt_ref(*new RectStyleValue(rect));
}

NonnullRefPtr<LengthStyleValue> LengthStyleValue::create(Length const& length)
{
    if (length.is_auto()) {
        static auto value = adopt_ref(*new LengthStyleValue(CSS::Length::make_auto()));
        return value;
    }
    if (length.is_px()) {
        if (length.raw_value() == 0) {
            static auto value = adopt_ref(*new LengthStyleValue(CSS::Length::make_px(0)));
            return value;
        }
        if (length.raw_value() == 1) {
            static auto value = adopt_ref(*new LengthStyleValue(CSS::Length::make_px(1)));
            return value;
        }
    }
    return adopt_ref(*new LengthStyleValue(length));
}

static Optional<CSS::Length> absolutized_length(CSS::Length const& length, CSSPixelRect const& viewport_rect, Gfx::FontPixelMetrics const& font_metrics, CSSPixels font_size, CSSPixels root_font_size)
{
    if (length.is_px())
        return {};
    if (length.is_absolute() || length.is_relative()) {
        auto px = length.to_px(viewport_rect, font_metrics, font_size, root_font_size);
        return CSS::Length::make_px(px);
    }
    return {};
}

NonnullRefPtr<StyleValue> StyleValue::absolutized(CSSPixelRect const&, Gfx::FontPixelMetrics const&, CSSPixels, CSSPixels) const
{
    return *this;
}

NonnullRefPtr<StyleValue> LengthStyleValue::absolutized(CSSPixelRect const& viewport_rect, Gfx::FontPixelMetrics const& font_metrics, CSSPixels font_size, CSSPixels root_font_size) const
{
    if (auto length = absolutized_length(m_length, viewport_rect, font_metrics, font_size, root_font_size); length.has_value())
        return LengthStyleValue::create(length.release_value());
    return *this;
}

NonnullRefPtr<StyleValue> ShadowStyleValue::absolutized(CSSPixelRect const& viewport_rect, Gfx::FontPixelMetrics const& font_metrics, CSSPixels font_size, CSSPixels root_font_size) const
{
    auto absolutized_offset_x = absolutized_length(m_offset_x, viewport_rect, font_metrics, font_size, root_font_size).value_or(m_offset_x);
    auto absolutized_offset_y = absolutized_length(m_offset_y, viewport_rect, font_metrics, font_size, root_font_size).value_or(m_offset_y);
    auto absolutized_blur_radius = absolutized_length(m_blur_radius, viewport_rect, font_metrics, font_size, root_font_size).value_or(m_blur_radius);
    auto absolutized_spread_distance = absolutized_length(m_spread_distance, viewport_rect, font_metrics, font_size, root_font_size).value_or(m_spread_distance);
    return ShadowStyleValue::create(m_color, absolutized_offset_x, absolutized_offset_y, absolutized_blur_radius, absolutized_spread_distance, m_placement);
}

NonnullRefPtr<StyleValue> BorderRadiusStyleValue::absolutized(CSSPixelRect const& viewport_rect, Gfx::FontPixelMetrics const& font_metrics, CSSPixels font_size, CSSPixels root_font_size) const
{
    if (m_horizontal_radius.is_percentage() && m_vertical_radius.is_percentage())
        return *this;
    auto absolutized_horizontal_radius = m_horizontal_radius;
    auto absolutized_vertical_radius = m_vertical_radius;
    if (!m_horizontal_radius.is_percentage())
        absolutized_horizontal_radius = absolutized_length(m_horizontal_radius.length(), viewport_rect, font_metrics, font_size, root_font_size).value_or(m_horizontal_radius.length());
    if (!m_vertical_radius.is_percentage())
        absolutized_vertical_radius = absolutized_length(m_vertical_radius.length(), viewport_rect, font_metrics, font_size, root_font_size).value_or(m_vertical_radius.length());
    return BorderRadiusStyleValue::create(absolutized_horizontal_radius, absolutized_vertical_radius);
}

bool CalculatedStyleValue::contains_percentage() const
{
    return m_expression->contains_percentage();
}

bool CalculatedStyleValue::CalcSum::contains_percentage() const
{
    if (first_calc_product->contains_percentage())
        return true;
    for (auto& part : zero_or_more_additional_calc_products) {
        if (part.contains_percentage())
            return true;
    }
    return false;
}

bool CalculatedStyleValue::CalcSumPartWithOperator::contains_percentage() const
{
    return value->contains_percentage();
}

bool CalculatedStyleValue::CalcProduct::contains_percentage() const
{
    if (first_calc_value.contains_percentage())
        return true;
    for (auto& part : zero_or_more_additional_calc_values) {
        if (part.contains_percentage())
            return true;
    }
    return false;
}

bool CalculatedStyleValue::CalcProductPartWithOperator::contains_percentage() const
{
    return value.visit(
        [](CalcValue const& value) { return value.contains_percentage(); },
        [](CalcNumberValue const&) { return false; });
}

bool CalculatedStyleValue::CalcValue::contains_percentage() const
{
    return value.visit(
        [](Percentage const&) { return true; },
        [](NonnullOwnPtr<CalcSum> const& sum) { return sum->contains_percentage(); },
        [](auto const&) { return false; });
}

bool calculated_style_value_contains_percentage(CalculatedStyleValue const& value)
{
    return value.contains_percentage();
}

}
