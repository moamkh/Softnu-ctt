--
-- PostgreSQL database dump
--

-- Dumped from database version 16.9 (Ubuntu 16.9-0ubuntu0.24.04.1)
-- Dumped by pg_dump version 16.9 (Ubuntu 16.9-0ubuntu0.24.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: call_func; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA call_func;


ALTER SCHEMA call_func OWNER TO postgres;

--
-- Name: user_func; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA user_func;


ALTER SCHEMA user_func OWNER TO postgres;

--
-- Name: checkcallsummaryexists(integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.checkcallsummaryexists(p_id integer) RETURNS boolean
    LANGUAGE plpgsql STABLE
    AS $$
DECLARE
    exists_flag BOOLEAN;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM public.call_summary WHERE id = p_id
    ) INTO exists_flag;

    RETURN exists_flag;
END;
$$;


ALTER FUNCTION call_func.checkcallsummaryexists(p_id integer) OWNER TO postgres;

--
-- Name: count_call_summaries(integer, text, text, integer, integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.count_call_summaries(p_user_id integer, p_search_field text DEFAULT NULL::text, p_search_value text DEFAULT NULL::text, p_start_date integer DEFAULT NULL::integer, p_end_date integer DEFAULT NULL::integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN (
        SELECT COUNT(*)
        FROM public.call_summary cs
        JOIN public.rel_user_calls ruc ON cs.id = ruc.call_summary_id
        WHERE 
            ruc.user_id = p_user_id -- Filter by user ID
            AND cs.type = 'external' -- Filter by type
            AND (
                p_search_field IS NULL OR p_search_value IS NULL -- No search
                OR (p_search_field = 'callee' AND cs.callee ILIKE '%' || p_search_value || '%')
                OR (p_search_field = 'all' AND (
                    cs.callee ILIKE '%' || p_search_value || '%'
                    OR cs.extension_number ILIKE '%' || p_search_value || '%'
                ))
            )
            AND (
                p_start_date IS NULL OR cs.start_time >= p_start_date
            )
            AND (
                p_end_date IS NULL OR cs.end_time <= p_end_date
            )
    );
END;
$$;


ALTER FUNCTION call_func.count_call_summaries(p_user_id integer, p_search_field text, p_search_value text, p_start_date integer, p_end_date integer) OWNER TO postgres;

--
-- Name: get_call_summaries(text, text, integer, integer, integer, integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.get_call_summaries(search_field text, search_value text, start_date integer, end_date integer, page integer, per_page integer) RETURNS TABLE(id integer, start_time integer, end_time integer, duration integer, callee character varying, direction character varying, type character varying, extension_number character varying, status character varying, ai_response text, unique_id character varying, created_at timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT 
        cs.id, cs.start_time, cs.end_time, cs.duration, cs.callee, 
        cs.direction, cs.type, cs.extension_number, cs.status, 
        cs.ai_response, cs.unique_id, cs.created_at
    FROM call_summary cs
    WHERE 
        (
            search_field IS NULL OR search_value IS NULL OR 
            (
                CASE 
                    WHEN search_field = 'callee' THEN cs.callee
                    WHEN search_field = 'extension_number' THEN cs.extension_number
                    WHEN search_field = 'direction' THEN cs.direction
                    WHEN search_field = 'type' THEN cs.type
                    ELSE NULL
                END
            ) = search_value
        )
        AND (start_date IS NULL OR cs.start_time >= start_date)
        AND (end_date IS NULL OR cs.end_time <= end_date)
    ORDER BY cs.created_at DESC
    LIMIT per_page OFFSET (page - 1) * per_page;
END;
$$;


ALTER FUNCTION call_func.get_call_summaries(search_field text, search_value text, start_date integer, end_date integer, page integer, per_page integer) OWNER TO postgres;

--
-- Name: get_call_summary_by_id(integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.get_call_summary_by_id(call_id integer) RETURNS TABLE(id integer, start_time integer, end_time integer, duration integer, callee character varying, direction character varying, type character varying, extension_number character varying, status character varying, ai_response text, unique_id character varying, created_at timestamp without time zone, user_id integer, fullname character varying, username character varying, is_active boolean)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT 
        cs.id, cs.start_time, cs.end_time, cs.duration, cs.callee, 
        cs.direction, cs.type, cs.extension_number, cs.status, 
        cs.ai_response, cs.unique_id, cs.created_at,
        u.id AS user_id, u.fullname, u.username, u.is_active
    FROM call_summary cs
    LEFT JOIN rel_user_calls ruc ON cs.id = ruc.call_summary_id
    LEFT JOIN users u ON ruc.user_id = u.id
    WHERE cs.id = call_id;
END;
$$;


ALTER FUNCTION call_func.get_call_summary_by_id(call_id integer) OWNER TO postgres;

--
-- Name: get_todays_call_summaries_with_empty_ai_response(); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.get_todays_call_summaries_with_empty_ai_response() RETURNS TABLE(call_summary_id integer, extension_number character varying, unique_id character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT 
        cs.id AS call_summary_id,
        cs.extension_number,
        cs.unique_id
    FROM 
        public.call_summary cs
    WHERE 
        cs.ai_response IS NULL 
        AND cs.unique_id IS NOT NULL
        AND cs.created_at::date = CURRENT_DATE -- Filter for today's calls
        AND cs.type = 'external'; -- Additional filter for 'external' type
END;
$$;


ALTER FUNCTION call_func.get_todays_call_summaries_with_empty_ai_response() OWNER TO postgres;

--
-- Name: get_user_call_summaries(integer, text, text, timestamp without time zone, timestamp without time zone, integer, integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.get_user_call_summaries(user_id integer, search_field text, search_value text, start_date timestamp without time zone, end_date timestamp without time zone, page integer, per_page integer) RETURNS TABLE(id integer, start_time integer, end_time integer, duration integer, callee character varying, direction character varying, type character varying, extension_number character varying, status character varying, ai_response text, unique_id character varying, created_at timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT 
        cs.id, cs.start_time, cs.end_time, cs.duration, cs.callee, 
        cs.direction, cs.type, cs.extension_number, cs.status, 
        cs.ai_response, cs.unique_id, cs.created_at
    FROM call_summary cs
    JOIN rel_user_calls ruc ON cs.id = ruc.call_summary_id
    WHERE ruc.user_id = user_id
        AND (search_field IS NULL OR (
            CASE 
                WHEN search_field = 'callee' THEN cs.callee
                WHEN search_field = 'extension_number' THEN cs.extension_number
                WHEN search_field = 'direction' THEN cs.direction
                WHEN search_field = 'type' THEN cs.type
                ELSE NULL
            END
        ) = search_value)
        AND (start_date IS NULL OR cs.created_at >= start_date)
        AND (end_date IS NULL OR cs.created_at <= end_date)
    ORDER BY cs.created_at DESC
    LIMIT per_page OFFSET (page - 1) * per_page;
END;
$$;


ALTER FUNCTION call_func.get_user_call_summaries(user_id integer, search_field text, search_value text, start_date timestamp without time zone, end_date timestamp without time zone, page integer, per_page integer) OWNER TO postgres;

--
-- Name: getcallsummarybyid(integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.getcallsummarybyid(p_id integer) RETURNS TABLE(id integer, start_time integer, end_time integer, duration integer, callee character varying, direction character varying, type character varying, extension_number character varying, status character varying, ai_response text, unique_id character varying, created_at timestamp without time zone)
    LANGUAGE plpgsql STABLE
    AS $$
BEGIN
    RETURN QUERY
    SELECT 
        cs.id,
        cs.start_time,
        cs.end_time,
        cs.duration,
        cs.callee,
        cs.direction,
        cs."type",
        cs.extension_number,
        cs.status,
        cs.ai_response,
        cs.unique_id,
        cs.created_at
    FROM public.call_summary cs
    WHERE cs.id = p_id;
END;
$$;


ALTER FUNCTION call_func.getcallsummarybyid(p_id integer) OWNER TO postgres;

--
-- Name: insert_ai_response(integer, text); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.insert_ai_response(callsummaryid integer, airesponse text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
    -- Update the call_summary table with the given AI response
    UPDATE public.call_summary
    SET ai_response = aiResponse
    WHERE id = callSummaryId;

    -- Check if the update was successful
    IF NOT FOUND THEN
        RAISE EXCEPTION 'No call_summary found with id %', callSummaryId;
    END IF;
END;
$$;


ALTER FUNCTION call_func.insert_ai_response(callsummaryid integer, airesponse text) OWNER TO postgres;

--
-- Name: list_call_summaries(integer, text, text, integer, integer, integer, integer); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.list_call_summaries(p_user_id integer, p_search_field text DEFAULT NULL::text, p_search_value text DEFAULT NULL::text, p_page integer DEFAULT 1, p_page_size integer DEFAULT 20, p_start_date integer DEFAULT NULL::integer, p_end_date integer DEFAULT NULL::integer) RETURNS TABLE(id integer, start_time integer, end_time integer, duration integer, callee character varying, direction character varying, type character varying, extension_number character varying, status character varying, ai_response text, unique_id character varying, created_at timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
DECLARE
    offset_val integer;
BEGIN
    -- Normalize empty strings to NULL
    IF p_search_field = '' THEN
        p_search_field := NULL;
    END IF;
    IF p_search_value = '' THEN
        p_search_value := NULL;
    END IF;

    -- Validate the search field if provided
    IF p_search_field IS NOT NULL THEN
        IF p_search_field NOT IN ('callee', 'all') THEN
            RAISE EXCEPTION 'Invalid search field. Allowed values are "callee" or "all".';
        END IF;
    END IF;

    -- Ensure page is at least 1
    IF p_page < 1 THEN
        p_page := 1;
    END IF;

    -- Calculate the offset for pagination
    offset_val := (p_page - 1) * p_page_size;

    -- Construct and execute the query
    RETURN QUERY
    SELECT 
        cs.id,
        cs.start_time,
        cs.end_time,
        cs.duration,
        cs.callee,
        cs.direction,
        cs.type,
        cs.extension_number,
        cs.status,
        cs.ai_response,
        cs.unique_id,
        cs.created_at
    FROM public.call_summary cs
    JOIN public.rel_user_calls ruc ON cs.id = ruc.call_summary_id
    WHERE 
        ruc.user_id = p_user_id -- Filter by user ID
        AND cs.type = 'external' -- Filter by type
        AND (
            p_search_field IS NULL OR p_search_value IS NULL -- No search
            OR (p_search_field = 'callee' AND cs.callee ILIKE '%' || p_search_value || '%')
            OR (p_search_field = 'all' AND (
                cs.callee ILIKE '%' || p_search_value || '%'
                OR cs.extension_number ILIKE '%' || p_search_value || '%'
            ))
        )
        AND (
            p_start_date IS NULL OR cs.start_time >= p_start_date
        )
        AND (
            p_end_date IS NULL OR cs.end_time <= p_end_date
        )
    ORDER BY cs.created_at DESC -- Sort by creation date (most recent first)
    LIMIT p_page_size OFFSET offset_val; -- Apply pagination
END;
$$;


ALTER FUNCTION call_func.list_call_summaries(p_user_id integer, p_search_field text, p_search_value text, p_page integer, p_page_size integer, p_start_date integer, p_end_date integer) OWNER TO postgres;

--
-- Name: store_call_summary(character varying, integer, integer, integer, character varying, character varying, character varying, character varying, character varying); Type: FUNCTION; Schema: call_func; Owner: postgres
--

CREATE FUNCTION call_func.store_call_summary(p_extension_number character varying, p_start_time integer, p_end_time integer, p_duration integer, p_callee character varying, p_direction character varying, p_type character varying, p_status character varying, p_unique_id character varying) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    user_id integer;
    new_call_summary_id integer;
BEGIN
    -- Find the user_id using the extension_number
    SELECT id INTO user_id
    FROM public.users
    WHERE extension_number = p_extension_number;

    -- If no user is found, raise an exception
    IF user_id IS NULL THEN
        RAISE EXCEPTION 'User with extension number % not found', p_extension_number;
    END IF;

    -- Insert a new call summary and get the ID
    INSERT INTO public.call_summary (
        start_time, 
        end_time, 
        duration, 
        callee, 
        direction, 
        type, 
        extension_number, 
        status,
        unique_id
    ) VALUES (
        p_start_time, 
        p_end_time, 
        p_duration, 
        p_callee, 
        p_direction, 
        p_type, 
        p_extension_number, 
        p_status,
        p_unique_id
    )
    RETURNING id INTO new_call_summary_id;

    -- Link the call summary to the user
    INSERT INTO public.rel_user_calls (user_id, call_summary_id)
    VALUES (user_id, new_call_summary_id);

    -- Return the new call_summary_id
    RETURN new_call_summary_id;
END;
$$;


ALTER FUNCTION call_func.store_call_summary(p_extension_number character varying, p_start_time integer, p_end_time integer, p_duration integer, p_callee character varying, p_direction character varying, p_type character varying, p_status character varying, p_unique_id character varying) OWNER TO postgres;

--
-- Name: count_users(text, text); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.count_users(p_search_field text DEFAULT NULL::text, p_search_value text DEFAULT NULL::text) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    sql TEXT := 'SELECT COUNT(*) FROM public.users';
    total_count INTEGER;
BEGIN
    -- Normalize empty strings to NULL
    IF p_search_field = '' THEN
        p_search_field := NULL;
    END IF;
    IF p_search_value = '' THEN
        p_search_value := NULL;
    END IF;

    -- Add WHERE clause if filters are provided
    IF p_search_field IS NOT NULL AND p_search_value IS NOT NULL THEN
        IF p_search_field = 'fullname' THEN
            sql := sql || ' WHERE fullname ILIKE ''%' || p_search_value || '%''';
        ELSIF p_search_field = 'extension_number' THEN
            sql := sql || ' WHERE extension_number = ''' || p_search_value || '''';
        END IF;
    END IF;

    -- Execute the query
    EXECUTE sql INTO total_count;

    RETURN total_count;
END;
$$;


ALTER FUNCTION user_func.count_users(p_search_field text, p_search_value text) OWNER TO postgres;

--
-- Name: create_user(character varying, character varying, character varying); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.create_user(p_fullname character varying, p_extension_number character varying, p_username character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
    INSERT INTO users (fullname, extension_number, username)
    VALUES (p_fullname, p_extension_number, p_username);
END;
$$;


ALTER FUNCTION user_func.create_user(p_fullname character varying, p_extension_number character varying, p_username character varying) OWNER TO postgres;

--
-- Name: get_user_details(integer); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.get_user_details(p_user_id integer) RETURNS TABLE(fullname character varying, extension_number character varying, username character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT fullname, extension_number, username
    FROM users
    WHERE id = p_user_id;
END;
$$;


ALTER FUNCTION user_func.get_user_details(p_user_id integer) OWNER TO postgres;

--
-- Name: list_all_users(); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.list_all_users() RETURNS TABLE(id integer, fullname character varying, extension_number character varying, username character varying, is_active boolean)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT users.id, users.fullname, users.extension_number, users.username, users.is_active
    FROM users
    ORDER BY users.extension_number;
END;
$$;


ALTER FUNCTION user_func.list_all_users() OWNER TO postgres;

--
-- Name: list_users(text, text, integer, integer); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.list_users(p_search_field text DEFAULT NULL::text, p_search_value text DEFAULT NULL::text, p_page integer DEFAULT 1, p_per_page integer DEFAULT 20) RETURNS TABLE(id integer, fullname character varying, extension_number character varying, is_active boolean, last_activity integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
    offset_val INTEGER;
BEGIN
    -- Normalize empty strings to NULL
    IF p_search_field = '' THEN
        p_search_field := NULL;
    END IF;
    IF p_search_value = '' THEN
        p_search_value := NULL;
    END IF;

    -- Validate p_search_field if provided
    IF p_search_field IS NOT NULL THEN
        IF p_search_field NOT IN ('fullname', 'extension_number', 'all') THEN
            RAISE EXCEPTION 'Invalid search field. Allowed values are "fullname", "extension_number", or "all".';
        END IF;
    END IF;

    -- Ensure p_page is at least 1
    IF p_page < 1 THEN
        p_page := 1;
    END IF;

    -- Calculate offset_val
    offset_val := (p_page - 1) * p_per_page;

    -- Execute the query with filtering, pagination, and the last activity calculation
    RETURN QUERY
    SELECT 
        u.id, 
        u.fullname, 
        u.extension_number, 
        u.is_active, 
        COALESCE(
            (
                SELECT MAX(cs.end_time)
                FROM public.call_summary cs
                JOIN public.rel_user_calls ruc ON ruc.call_summary_id = cs.id
                WHERE ruc.user_id = u.id AND cs.type = 'external'
            ), 
            0
        ) AS last_activity
    FROM public.users u
    WHERE 
        (p_search_field IS NULL OR p_search_value IS NULL) -- No filter
        OR (p_search_field = 'fullname' AND u.fullname ILIKE '%' || p_search_value || '%')
        OR (p_search_field = 'extension_number' AND u.extension_number ILIKE '%' || p_search_value || '%')
        OR (p_search_field = 'all' AND 
            (u.fullname ILIKE '%' || p_search_value || '%' OR u.extension_number ILIKE '%' || p_search_value || '%'))
    ORDER BY u.id
    LIMIT p_per_page OFFSET offset_val;
END;
$$;


ALTER FUNCTION user_func.list_users(p_search_field text, p_search_value text, p_page integer, p_per_page integer) OWNER TO postgres;

--
-- Name: update_user(integer, character varying, character varying, character varying); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.update_user(p_user_id integer, p_fullname character varying DEFAULT NULL::character varying, p_extension_number character varying DEFAULT NULL::character varying, p_username character varying DEFAULT NULL::character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF p_fullname IS NOT NULL THEN
        UPDATE users
        SET fullname = p_fullname
        WHERE id = p_user_id;
    END IF;

    IF p_extension_number IS NOT NULL THEN
        UPDATE users
        SET extension_number = p_extension_number
        WHERE id = p_user_id;
    END IF;

    IF p_username IS NOT NULL THEN
        UPDATE users
        SET username = p_username
        WHERE id = p_user_id;
    END IF;
END;
$$;


ALTER FUNCTION user_func.update_user(p_user_id integer, p_fullname character varying, p_extension_number character varying, p_username character varying) OWNER TO postgres;

--
-- Name: update_user_is_active(integer, boolean); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.update_user_is_active(p_user_id integer, p_is_active boolean) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
    UPDATE users
    SET is_active = p_is_active
    WHERE id = p_user_id;
END;
$$;


ALTER FUNCTION user_func.update_user_is_active(p_user_id integer, p_is_active boolean) OWNER TO postgres;

--
-- Name: user_exists(integer); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.user_exists(p_user_id integer) RETURNS boolean
    LANGUAGE plpgsql
    AS $$
DECLARE
    exists_flag BOOLEAN;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM users WHERE id = p_user_id
    ) INTO exists_flag;

    RETURN exists_flag;
END;
$$;


ALTER FUNCTION user_func.user_exists(p_user_id integer) OWNER TO postgres;

--
-- Name: user_exists_by_username(character varying); Type: FUNCTION; Schema: user_func; Owner: postgres
--

CREATE FUNCTION user_func.user_exists_by_username(p_username character varying) RETURNS boolean
    LANGUAGE plpgsql
    AS $$
DECLARE
    exists_flag BOOLEAN;
BEGIN
    SELECT EXISTS (
        SELECT 1 FROM users WHERE username = p_username
    ) INTO exists_flag;

    RETURN exists_flag;
END;
$$;


ALTER FUNCTION user_func.user_exists_by_username(p_username character varying) OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: call_summary; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.call_summary (
    id integer NOT NULL,
    start_time integer NOT NULL,
    end_time integer NOT NULL,
    duration integer NOT NULL,
    callee character varying(50) NOT NULL,
    direction character varying(10) NOT NULL,
    type character varying(20),
    extension_number character varying(50) NOT NULL,
    status character varying(255) DEFAULT NULL::character varying,
    ai_response text,
    unique_id character varying(255),
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.call_summary OWNER TO postgres;

--
-- Name: call_summary_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.call_summary_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.call_summary_id_seq OWNER TO postgres;

--
-- Name: call_summary_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.call_summary_id_seq OWNED BY public.call_summary.id;


--
-- Name: rel_user_calls; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.rel_user_calls (
    id integer NOT NULL,
    user_id integer NOT NULL,
    call_summary_id integer NOT NULL
);


ALTER TABLE public.rel_user_calls OWNER TO postgres;

--
-- Name: rel_user_calls_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.rel_user_calls_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.rel_user_calls_id_seq OWNER TO postgres;

--
-- Name: rel_user_calls_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.rel_user_calls_id_seq OWNED BY public.rel_user_calls.id;


--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    id integer NOT NULL,
    fullname character varying(255),
    extension_number character varying(50) NOT NULL,
    username character varying(50),
    is_active boolean DEFAULT true
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.users_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.users_id_seq OWNER TO postgres;

--
-- Name: users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.users_id_seq OWNED BY public.users.id;


--
-- Name: call_summary id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.call_summary ALTER COLUMN id SET DEFAULT nextval('public.call_summary_id_seq'::regclass);


--
-- Name: rel_user_calls id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.rel_user_calls ALTER COLUMN id SET DEFAULT nextval('public.rel_user_calls_id_seq'::regclass);


--
-- Name: users id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users ALTER COLUMN id SET DEFAULT nextval('public.users_id_seq'::regclass);


--
-- Name: call_summary call_summary_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.call_summary
    ADD CONSTRAINT call_summary_pkey PRIMARY KEY (id);


--
-- Name: rel_user_calls rel_user_calls_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.rel_user_calls
    ADD CONSTRAINT rel_user_calls_pkey PRIMARY KEY (id);


--
-- Name: users unique_extension_number; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT unique_extension_number UNIQUE (extension_number);


--
-- Name: users unique_fullname; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT unique_fullname UNIQUE (fullname);


--
-- Name: users unique_username; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT unique_username UNIQUE (username);


--
-- Name: users users_extension_number_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_extension_number_key UNIQUE (extension_number);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- Name: rel_user_calls rel_user_calls_call_summary_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.rel_user_calls
    ADD CONSTRAINT rel_user_calls_call_summary_id_fkey FOREIGN KEY (call_summary_id) REFERENCES public.call_summary(id) ON DELETE CASCADE;


--
-- Name: rel_user_calls rel_user_calls_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.rel_user_calls
    ADD CONSTRAINT rel_user_calls_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id);


--
-- PostgreSQL database dump complete
--

