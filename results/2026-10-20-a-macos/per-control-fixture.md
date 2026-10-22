# Per-control results: the stock-control fixture

Both passes (initial and after the default actions) are counted, so a control with one node shows 2 / 2. "AX client" counts nodes per control type, including controls nested inside another control (a dock panel's text box, a property grid's editors), so it can exceed the in-process count; "n/a" there means the type occurs only nested, where the client does not act. A default action of `none` means the control publishes no enabled action other than focus; `(no node)` means the control publishes nothing.

| Control (manifest id) | In process: nodes matched | Default action | Observed in process | AX client: nodes matched | Observed through AX |
|---|---|---|---|---|---|
| `review_card` | 0 / 0 | (no node) | n/a | – | n/a |
| `range_control` | 0 / 0 | (no node) | n/a | – | n/a |
| `scrollable_control` | 0 / 0 | (no node) | n/a | – | n/a |
| `visual_inspector_overlay` | 0 / 0 | (no node) | n/a | – | n/a |
| `button` | 2 / 2 | press | yes | 72 / 72 | yes |
| `drop_down_button` | 2 / 2 | press | yes | 2 / 2 | yes |
| `link_label` | 2 / 2 | press | yes | 2 / 2 | yes |
| `command_bar` | 6 / 6 | press | yes | 2 / 2 | yes |
| `label` | 2 / 2 | none | n/a | 16 / 16 | n/a |
| `text_box` | 2 / 2 | set_value | yes | 26 / 26 | yes |
| `token_box` | 8 / 8 | set_value | yes | 6 / 6 | yes |
| `masked_text_box` | 2 / 2 | set_value | yes | 2 / 2 | yes |
| `picture_box` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `progress_bar` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `card` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `check_box` | 2 / 2 | press | yes | 16 / 16 | yes |
| `radio_button` | 2 / 2 | press | yes | 2 / 2 | yes |
| `track_bar` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `numeric_up_down` | 2 / 2 | increment | yes | 4 / 4 | yes |
| `domain_up_down` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `color_value_editor` | 4 / 4 | set_value | yes | 4 / 4 | yes |
| `flags_value_editor` | 2 / 2 | expand | yes | 4 / 4 | yes |
| `property_list` | 5 / 5 | collapse | yes | 31 / 31 | yes |
| `date_time_picker` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `month_calendar` | 68 / 68 | set_value | yes | 68 / 68 | yes |
| `instrument_rack` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `list_box` | 8 / 8 | press | yes | 24 / 24 | yes |
| `checked_list_box` | 6 / 6 | press | yes | 6 / 6 | yes |
| `combo_box` | 2 / 2 | expand | yes | 10 / 10 | yes |
| `tree_view` | 7 / 7 | press | yes | 8 / 8 | yes |
| `object_view` | 6 / 6 | press | yes | 6 / 6 | yes |
| `list_view` | 10 / 10 | press | yes | 10 / 10 | yes |
| `tree_list_view` | 10 / 10 | press | yes | 10 / 10 | yes |
| `correspondence_view` | 10 / 10 | press | yes | 10 / 10 | yes |
| `data_grid_view` | 12 / 12 | press | yes | 12 / 12 | yes |
| `choice_group` | 6 / 6 | press | yes | 2 / 2 | yes |
| `breadcrumb_trail` | 8 / 8 | press | yes | 8 / 8 | yes |
| `panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `group_box` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `scaled_group_box` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `material_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `expandable_sections` | 4 / 4 | press | yes | 2 / 2 | yes |
| `container_control` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `user_control` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `tab_control` | 6 / 6 | select | yes | 6 / 6 | yes |
| `tab_page` | 2 / 2 | select | yes | 2 / 2 | n/a |
| `page_view` | 12 / 12 | select | yes | 12 / 12 | yes |
| `page_strip` | 6 / 6 | select | yes | 16 / 16 | yes |
| `navigation_pane` | 14 / 14 | press | yes | 13 / 13 | yes |
| `flow_layout_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `table_layout_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `responsive_track_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `scaled_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `split_container` | 2 / 2 | increment | yes | 4 / 4 | yes |
| `splitter_panel` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `command_overflow_panel` | 4 / 4 | press | yes | 2 / 2 | yes |
| `master_detail_view` | 4 / 4 | none | n/a | 2 / 2 | n/a |
| `scroll_bar` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `h_scroll_bar` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `v_scroll_bar` | 2 / 2 | increment | yes | 2 / 2 | yes |
| `menu_strip` | 4 / 4 | press | yes | 4 / 4 | yes |
| `tool_strip` | 4 / 4 | press | yes | 8 / 8 | yes |
| `status_strip` | 4 / 4 | press | yes | 4 / 4 | yes |
| `quick_access_toolbar` | 6 / 6 | press | yes | 6 / 6 | yes |
| `tool_strip_panel` | 4 / 4 | press | yes | – | n/a |
| `tool_strip_container` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `binding_navigator` | 22 / 22 | none | n/a | 20 / 20 | n/a |
| `ribbon` | 14 / 14 | press | yes | 14 / 14 | yes |
| `backstage_view` | 8 / 8 | press | yes | 6 / 6 | yes |
| `ribbon_gallery` | 14 / 14 | press | yes | 14 / 14 | yes |
| `ribbon_font_control` | 6 / 6 | expand | yes | 2 / 2 | yes |
| `ribbon_color_picker` | 2 / 2 | press | yes | 2 / 2 | yes |
| `dock_site` | 26 / 26 | set_value | yes | 4 / 4 | yes |
| `dock_navigator` | 10 / 10 | press | yes | 10 / 10 | yes |
| `property_grid` | 106 / 106 | set_value | yes | – | n/a |
| `anchored_popup_layer` | 4 / 4 | none | n/a | 2 / 2 | n/a |
| `font_dialog` | 32 / 32 | press | yes | 2 / 2 | yes |
| `task_dialog` | 8 / 8 | press | yes | 2 / 2 | yes |
| `drawing_surface` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `raster_canvas` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `easing_preview` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `metrics_view` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `typography_specimen` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `visual_inspector_view` | 2 / 2 | none | n/a | 2 / 2 | n/a |
| `control:page_view.panel.summary` | – | – | n/a | 1 / 1 | n/a |
| `control:navigation_pane.content` | – | – | n/a | 1 / 1 | n/a |
| `control:dock_site.stack.2.panel.notes` | – | – | n/a | 2 / 2 | n/a |
| `control:dock_site.stack.3.panel.toolbox` | – | – | n/a | 2 / 2 | n/a |
| `control:page_view.panel.notes` | – | – | n/a | 1 / 1 | n/a |
